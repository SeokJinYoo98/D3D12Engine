#include "Common\pch.h"
#include "Renderer.h"
#include "Gfw/GameScene.h"

CRenderer::CRenderer()
{
}
void CRenderer::InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight)
{
	UINT nMesh = 5, nPass = 1, nFrame = 1;

	CD3D12Device::InitDevice(hWnd, nWidth, nHeight);
	CD3D12Device::CreateCbvDescriptorHeap(nMesh, nPass, nFrame);
	BuildConstantBufferViews(nMesh, nPass, nFrame);

	BuildResourceManager();
}
void CRenderer::Render(CGameScene* pGameScene)
{
	CD3D12Device::ResetCommandAlloc();
	CD3D12Device::TransitionResourceFromPresentToRenderTarget();
	CD3D12Device::ClearRenderTargetAndDepthStencil(pGameScene->GetBgColor());

	RenderScene(pGameScene);

	CD3D12Device::TransitionRenderTargetToPresent();
	CD3D12Device::ExcuteCommandList();
	CD3D12Device::WaitForGpuComplete();
	CD3D12Device::PresentSwapChain();
	CD3D12Device::MoveToNextFrame();
}

void CRenderer::Update(CGameScene* pGameScene)
{
	UpdatePassCB(pGameScene);
	UpdateMeshCB(pGameScene);
}
void CRenderer::UpdatePassCB(CGameScene* pGameScene)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	cd3dMeshGpuHandle.Offset(m_nPassOffset, m_nCbvDescriptorSize);

	auto camera = pGameScene->GetCamera();
	m_mainPassCB.xmf4x4Projection	= Matrix4x4::Transpose(camera->GetProj());
	m_mainPassCB.xmf4x4View			= Matrix4x4::Transpose(camera->GetView());

	m_pPassCBs->CopyData(0, m_mainPassCB);
}

void CRenderer::UpdateMeshCB(CGameScene* pGameScene)
{
	const auto& pMeshes = pGameScene->GetMeshes();
	UINT meshIndex = -1;

	for (const auto& [name, meshes] : pMeshes) {
		for (const auto& mesh : meshes) {
			++meshIndex;
			CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
			cd3dMeshGpuHandle.Offset(meshIndex, m_nCbvDescriptorSize);
			MeshConstants meshConst;
			meshConst.xmf4Color = mesh->GetColor();
			meshConst.xmf4x4World = Matrix4x4::Transpose(mesh->GetTransform());
			m_pMeshCBs->CopyData(meshIndex, meshConst);
		}
	}
}

void CRenderer::BuildResourceManager()
{
	auto& resManager = CResourceManager::GetInstance();
	CD3D12Device::ResetCommandList();
	resManager.InitBegin(m_pDevice.Get(), m_pCommandList.Get());
	CD3D12Device::ExcuteCommandList();
	CD3D12Device::WaitForGpuComplete();
	resManager.InitEnd();
}

void CRenderer::BuildFrameResource(UINT nMesh, UINT nPass, UINT nFrame)
{
	for (UINT i = 0; i < nFrame; ++i) {
		m_pFrameResources.push_back(std::make_unique<CFrameResource>(m_pDevice.Get(), nMesh, nPass));
	}
	m_pCurrFrameResource = m_pFrameResources[0].get();
	m_nCurrFrameResourceIndex = 0;
}

void CRenderer::BuildConstantBufferViews(UINT nMesh, UINT nPass, UINT nFrame)
{
	auto pDevice = m_pDevice.Get();

	m_pMeshCBs = std::make_unique<CUploadBuffer<MeshConstants>>(pDevice, nMesh, true);
	UINT nMeshCbByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(MeshConstants));
	auto pMeshCB = m_pMeshCBs->Resource();
	for (UINT i = 0; i < nMesh; ++i) 
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dCbAddress = pMeshCB->GetGPUVirtualAddress();
		d3dCbAddress += (i * nMeshCbByteSize);
		int nHeapIndex = i;

		auto cd3dMeshHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetCPUDescriptorHandleForHeapStart());
		cd3dMeshHandle.Offset(nHeapIndex, m_nCbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCbvDesc{ };
		d3dCbvDesc.BufferLocation = d3dCbAddress;
		d3dCbvDesc.SizeInBytes = nMeshCbByteSize;

		pDevice->CreateConstantBufferView(&d3dCbvDesc, cd3dMeshHandle);
	}

	m_nPassOffset = nMesh;
	m_pPassCBs = std::make_unique<CUploadBuffer<PassConstants>>(pDevice, nPass, true);
	UINT nPassCbByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(PassConstants));
	auto pPassCB = m_pPassCBs->Resource();
	for (UINT i = 0; i < nMesh; ++i) 
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dCbAddress = pPassCB->GetGPUVirtualAddress();
		int nHeapIndex = m_nPassOffset;
		auto cd3dHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetCPUDescriptorHandleForHeapStart());
		cd3dHandle.Offset(nHeapIndex, m_nCbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCbvDesc{ };
		d3dCbvDesc.BufferLocation = d3dCbAddress;
		d3dCbvDesc.SizeInBytes = nPassCbByteSize;
		pDevice->CreateConstantBufferView(&d3dCbvDesc, cd3dHandle);
	}
}


void CRenderer::RenderScene(CGameScene* pGameScene)
{
	auto& resManager = CResourceManager::GetInstance();
	// 루트 시그니처
	m_pCommandList->SetGraphicsRootSignature(resManager.LoadRootSig("Default"));

	// 뷰포트, 렉트
	auto pCamera = pGameScene->GetCamera();
	m_pCommandList->RSSetViewports(1, pCamera->GetViewport());
	m_pCommandList->RSSetScissorRects(1, pCamera->GetScissorRect());

	// 디스크립터 힙을 바인딩
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pCbvHeap.Get() };
	m_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// 패스 테이블 바인딩
	CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dPassGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	cd3dPassGpuHandle.Offset(m_nPassOffset, m_nCbvDescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1, cd3dPassGpuHandle);

	std::string currPsoName = "None";
	std::string currMeshName = "None";
	CMesh*		pCurrMesh = nullptr;
	const auto& pMeshes = pGameScene->GetMeshes();
	UINT		nMeshIndex = -1;
	for (const auto& [psoName, meshes] : pMeshes) {
		if (currPsoName != psoName) {
			m_pCommandList->SetPipelineState(resManager.LoadPSO(psoName));
			currPsoName = psoName;
		}
		
		for (const auto& mesh : meshes) {
			++nMeshIndex;
			if (currMeshName != mesh->m_strMeshName) {
				pCurrMesh = resManager.LoadMesh(mesh->m_strMeshName);
				currMeshName = mesh->m_strMeshName;
			}
		}
		CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cd3dMeshGpuHandle.Offset(nMeshIndex, m_nCbvDescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(0, cd3dMeshGpuHandle);
		pCurrMesh->Render(m_pCommandList.Get());
	}
}
