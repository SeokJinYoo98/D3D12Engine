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
	//std::cout << "Build" << std::endl;
	BuildConstantBufferViews(nMesh, nPass, nFrame);

	BuildResourceManager();
}
void CRenderer::Render(CGameScene* pGameScene)
{
	Update(pGameScene);
	BeginRendering(pGameScene->GetBgColor());
	RenderScene(pGameScene);
	EndRendering();
}

void CRenderer::Update(CGameScene* pGameScene)
{
	//std::cout << "Update" << std::endl;
	UpdatePassCB(pGameScene);
	UpdateMeshCB(pGameScene);
}



void CRenderer::BeginRendering(const DirectX::XMFLOAT4& xmf4BgColor)
{
	ResetCommandAlloc();
	TransitionResourceFromPresentToRenderTarget();
	ClearRenderTargetAndDepthStencil(xmf4BgColor);

	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_pCbvHeap.Get() };
	//m_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//m_pCommandList->SetGraphicsRootDescriptorTable(1, m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
}

void CRenderer::EndRendering()
{
	TransitionRenderTargetToPresent();
	ExcuteCommandList();
	WaitForGpuComplete();
	PresentSwapChain();
	MoveToNextFrame();
}

void CRenderer::UpdatePassCB(CGameScene* pGameScene)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	cd3dMeshGpuHandle.Offset(m_nPassOffset, m_nCbvDescriptorSize);
	//std::cout << "GPU Handle PTR: " << cd3dMeshGpuHandle.ptr << ", Resource GPU PTR: " << m_pPassCBs->Resource()->GetGPUVirtualAddress() << ", Pass[" << "5" << "]" << std::endl;
	auto camera = pGameScene->GetCamera();
	m_mainPassCB.xmf4x4Projection	= Matrix4x4::Transpose(camera->GetProj());
	m_mainPassCB.xmf4x4View			= Matrix4x4::Transpose(camera->GetView());

	m_pPassCBs->CopyData(0, m_mainPassCB);
}

void CRenderer::UpdateMeshCB(CGameScene* pGameScene)
{
	UINT nMeshCbByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(MeshConstants));
	const auto& pMeshes = pGameScene->GetMeshComps();

	for (UINT i = 0; i < pMeshes.size(); ++i) {
		CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cd3dMeshGpuHandle.Offset(i, m_nCbvDescriptorSize);
		MeshConstants meshConst;
		meshConst.xmf4Color = pMeshes[i]->GetColor();
		meshConst.xmf4x4World = Matrix4x4::Transpose(pMeshes[i]->GetTransform());
		m_pMeshCBs->CopyData(i, meshConst);
		//std::cout << "GPU Handle PTR: " << cd3dMeshGpuHandle.ptr << ", Resource GPU PTR: " << m_pMeshCBs->Resource()->GetGPUVirtualAddress() + (nMeshCbByteSize * i) << ", Mesh[" << i << "]" << std::endl;
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

	//for (UINT i = 0; i < nMesh; ++i) {
	//	auto cd3dMeshHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	//	cd3dMeshHandle.Offset(i, m_nCbvDescriptorSize);
	//	std::cout << "GPU Handle PTR: " << cd3dMeshHandle.ptr << ", Resource GPU PTR: " << m_pMeshCBs->Resource()->GetGPUVirtualAddress() + (nMeshCbByteSize * i) << ", Mesh[" << i << "]" << std::endl;
	//	
	//}
	//D3D12_GPU_VIRTUAL_ADDRESS d3dCbAddress = pPassCB->GetGPUVirtualAddress();
	//auto cd3dHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	//cd3dHandle.Offset(m_nPassOffset, m_nCbvDescriptorSize);

	//std::cout << "GPU Handle PTR: " << cd3dHandle.ptr << ", Resource GPU PTR: " << m_pPassCBs->Resource()->GetGPUVirtualAddress()  << ", Pass[" << m_nPassOffset << "]" << std::endl;
}

void CRenderer::SetViewportsAndScissorRects(CCamera* pCamera)
{

}

void CRenderer::RenderScene(CGameScene* pGameScene)
{
	//std::cout << "Render" << std::endl;
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
	//std::cout << "GPU Handle PTR: " << cd3dPassGpuHandle.ptr << ", Resource GPU PTR: " << m_pPassCBs->Resource()->GetGPUVirtualAddress() << ", Pass[" << m_nPassOffset << "]" << std::endl;
	
	//UINT nMeshCbByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(MeshConstants));
	auto pPso = resManager.LoadPSO("Opaque");
	const auto& pMeshes = pGameScene->GetMeshComps();
	for (int i = 0; i < (int)(pMeshes.size()); ++i) {
		CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cd3dMeshGpuHandle.Offset(i, m_nCbvDescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(0, cd3dMeshGpuHandle);

		m_pCommandList->SetPipelineState(pPso);
		auto pMesh = resManager.LoadMesh(pMeshes[i]->m_strMeshName);
		pMesh->Render(m_pCommandList.Get());
		//std::cout << "GPU Handle PTR: " << cd3dMeshGpuHandle.ptr << ", Resource GPU PTR: " << m_pMeshCBs->Resource()->GetGPUVirtualAddress() + (nMeshCbByteSize * i) << ", Mesh[" << i << "]" << std::endl;
	}
}
