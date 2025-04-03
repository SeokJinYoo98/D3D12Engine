#include "Common\pch.h"
#include "Renderer.h"
#include "Gfw/GameScene/GameScene.h"

CRenderer::CRenderer()
{
}
void CRenderer::InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight)
{
	UINT nMesh = 100, nPass = 1, nUI = 0;

	CD3D12Device::InitDevice(hWnd, nWidth, nHeight);
	CD3D12Device::CreateCbvDescriptorHeap(nMesh, nUI, nPass);
	BuildConstantBufferViews(nMesh, nUI, nPass);
	BuildResourceManager();
	BuildGrid();
}
void CRenderer::Render(CGameScene* pGameScene, bool bDrawGrid)
{
	CD3D12Device::ResetCommandAlloc();
	CD3D12Device::TransitionResourceFromPresentToRenderTarget();
	CD3D12Device::ClearRenderTargetAndDepthStencil(Vector4::StoreFloat4(DirectX::Colors::Black));

	RenderScene(pGameScene);
	//DrawGrid(bDrawGrid);

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
	auto camera = pGameScene->GetCamera();
	m_mainPassCB.xmf4x4Projection	= Matrix4x4::Transpose(camera->GetProj());
	m_mainPassCB.xmf4x4View			= Matrix4x4::Transpose(camera->GetView());
	m_mainPassCB.xmf3EyePosition	= camera->GetEyePos();
	m_mainPassCB.xmf4AmbientLight	= pGameScene->GetAmbientLight();

	auto& directionalLights = pGameScene->GetDirectionalLights();
	m_mainPassCB.nNumDirectionalLights = static_cast<int>(directionalLights.size());
	for (size_t i = 0; i < directionalLights.size(); ++i) {
		if (i >= MAX_DIRECTIONALLIGHT) break;
		m_mainPassCB.DirectionalLight[i] = directionalLights[i].GetLightData();
	}
	
	auto& pointLights = pGameScene->GetPointLights();
	for (size_t i = 0; i < pointLights.size(); ++i) {
		if (i >= MAX_POINTLIGHT) break;
		m_mainPassCB.PointLights[i] = pointLights[i].GetPointLightData();
	}
	m_mainPassCB.nNumPointLights = static_cast<int>(pointLights.size());

	m_pPassCBs->CopyData(0, m_mainPassCB);
}

void CRenderer::UpdateMeshCB(CGameScene* pGameScene)
{
	UINT meshIndex = m_nMeshOffset;
	for (const auto& [name, meshes] : pGameScene->GetMeshes()) {
		for (const auto& mesh : meshes) {
			++meshIndex;
			MeshConstants meshConst;
			meshConst.xmf4x4World = Matrix4x4::Transpose(mesh->GetTransform());
			meshConst.xmf4x4WorldInv = Matrix4x4::Transpose(Matrix4x4::Inverse(mesh->GetTransform()));
			meshConst.xmf4Color = mesh->m_xmf4Color;
			m_pMeshCBs->CopyData(meshIndex, meshConst);

			MaterialConstants matConst;
			matConst.fRoughness = mesh->m_pMaterial->fRoughness;
			matConst.xmf3FresnelR0 = mesh->m_pMaterial->xmf3FresnelR0;
			matConst.xmf4DiffuseAlbedo = mesh->m_pMaterial->xmf4DiffuseAlbedo;
			m_pMaterialCBs->CopyData(meshIndex, matConst);
		}
	}
}

void CRenderer::UpdateMaterialCB()
{
}

void CRenderer::BuildResourceManager()
{
	m_pResourceManager = std::make_unique<CResourceManager>();
	CD3D12Device::ResetCommandList();
	m_pResourceManager->InitBegin(m_pDevice.Get(), m_pCommandList.Get());
	CD3D12Device::ExcuteCommandList();
	CD3D12Device::WaitForGpuComplete();
	m_pResourceManager->InitEnd();
}

void CRenderer::BuildConstantBufferViews(UINT nMesh, UINT nUI, UINT nPass)
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

	m_pMaterialCBs = std::make_unique<CUploadBuffer<MaterialConstants>>(pDevice, nMesh, true);
	UINT nMaterialCbByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	m_nMaterialCBOffset = nMesh;
	auto pMatCB = m_pMaterialCBs->Resource();
	for (UINT i = 0; i < nMesh; ++i)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dCbAddress = pMatCB->GetGPUVirtualAddress();
		d3dCbAddress += (i * nMaterialCbByteSize);
		int nHeapIndex = m_nMaterialCBOffset + i; // nMesh부터 시작

		auto cd3dMeshHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetCPUDescriptorHandleForHeapStart());
		cd3dMeshHandle.Offset(nHeapIndex, m_nCbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCbvDesc{};
		d3dCbvDesc.BufferLocation = d3dCbAddress;
		d3dCbvDesc.SizeInBytes = nMaterialCbByteSize;

		pDevice->CreateConstantBufferView(&d3dCbvDesc, cd3dMeshHandle);
	}

	m_nPassOffset = nMesh + nMesh;
	m_pPassCBs = std::make_unique<CUploadBuffer<PassConstants>>(pDevice, nPass, true);
	UINT nPassCbByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(PassConstants));
	auto pPassCB = m_pPassCBs->Resource();
	for (UINT i = 0; i < nPass; ++i)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dCbAddress = pPassCB->GetGPUVirtualAddress();
		d3dCbAddress += (i * nPassCbByteSize);
		int nHeapIndex = m_nPassOffset + i; // m_nPassOffset 이후로 배치

		auto cd3dHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetCPUDescriptorHandleForHeapStart());
		cd3dHandle.Offset(nHeapIndex, m_nCbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCbvDesc{};
		d3dCbvDesc.BufferLocation = d3dCbAddress;
		d3dCbvDesc.SizeInBytes = nPassCbByteSize;
		pDevice->CreateConstantBufferView(&d3dCbvDesc, cd3dHandle);
	}
}

void CRenderer::BuildGrid()
{
	//DirectX::XMFLOAT3 xmf3Scale{ 0.05f, 0.05f, 100.f };
	//// x축
	//m_gridConstants[0].xmf4x4World	= Matrix4x4::Identity();
	//auto mtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{0.f, 1.f, 0.f}, DirectX::XMConvertToRadians(90.f));
	//m_gridConstants[0].xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_gridConstants[0].xmf4x4World);
	//auto scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	//m_gridConstants[0].xmf4x4World = Matrix4x4::Multiply(scale, m_gridConstants[0].xmf4x4World);

	//// z축
	//m_gridConstants[1].xmf4x4World = Matrix4x4::Identity();
	//mtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ 0.f, 0.f, 1.f }, DirectX::XMConvertToRadians(90.f));
	//m_gridConstants[1].xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_gridConstants[1].xmf4x4World);
	//scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	//m_gridConstants[1].xmf4x4World = Matrix4x4::Multiply(scale, m_gridConstants[1].xmf4x4World);

	//// y축
	//m_gridConstants[2].xmf4x4World = Matrix4x4::Identity();
	//mtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ 1.f, 0.f, 0.f }, DirectX::XMConvertToRadians(90.f));
	//m_gridConstants[2].xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_gridConstants[2].xmf4x4World);
	//scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	//m_gridConstants[2].xmf4x4World = Matrix4x4::Multiply(scale, m_gridConstants[2].xmf4x4World);

	//for (size_t i = 0; i < m_gridConstants.size(); ++i) {
	//	m_gridConstants[i].xmf4x4World = Matrix4x4::Transpose(m_gridConstants[i].xmf4x4World);
	//	m_pMeshCBs->CopyData((int)i, m_gridConstants[i]);
	//}
	//auto gridMatX = m_pResourceManager->LoadMat("NoRefRed");
	//auto gridMatZ = m_pResourceManager->LoadMat("NoRefBlue");
	//auto gridMatY = m_pResourceManager->LoadMat("NoRefGreen");

	//MaterialConstants mat;
	//mat.fRoughness = gridMatX->fRoughness;
	//mat.xmf3FresnelR0 = gridMatX->xmf3FresnelR0;
	//mat.xmf4DiffuseAlbedo = gridMatX->xmf4DiffuseAlbedo;
	//m_pMaterialCBs->CopyData(0, mat);

	//mat.fRoughness = gridMatZ->fRoughness;
	//mat.xmf3FresnelR0 = gridMatZ->xmf3FresnelR0;
	//mat.xmf4DiffuseAlbedo = gridMatZ->xmf4DiffuseAlbedo;
	//m_pMaterialCBs->CopyData(1, mat);

	//mat.fRoughness = gridMatY->fRoughness;
	//mat.xmf3FresnelR0 = gridMatY->xmf3FresnelR0;
	//mat.xmf4DiffuseAlbedo = gridMatY->xmf4DiffuseAlbedo;
	//m_pMaterialCBs->CopyData(2, mat);

	//m_nMeshOffset = 2;
}


void CRenderer::RenderScene(CGameScene* pGameScene)
{
	// 루트 시그니처
	m_pCommandList->SetGraphicsRootSignature(m_pResourceManager->LoadRootSig("Default"));

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
	m_pCommandList->SetGraphicsRootDescriptorTable(2, cd3dPassGpuHandle);

	std::string		currPsoName = "None";

	UINT			nMeshIndex = m_nMeshOffset;

	for (const auto& [psoName, meshes] : pGameScene->GetMeshes()) {
		if (currPsoName != psoName) {
			m_pCommandList->SetPipelineState(m_pResourceManager->LoadPSO(psoName));
			currPsoName = psoName;
		}
		for (const auto& mesh : meshes) {
			++nMeshIndex;
			CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
			cd3dMeshGpuHandle.Offset(nMeshIndex, m_nCbvDescriptorSize);
			m_pCommandList->SetGraphicsRootDescriptorTable(0, cd3dMeshGpuHandle);

			cd3dMeshGpuHandle.Offset(m_nMaterialCBOffset, m_nCbvDescriptorSize);
			m_pCommandList->SetGraphicsRootDescriptorTable(1, cd3dMeshGpuHandle);

			mesh->m_pMesh->Render(m_pCommandList.Get());
		}
	}
}

void CRenderer::DrawGrid(bool bDrawGrid)
{
	if (!bDrawGrid) return;
	auto mesh = m_pResourceManager->LoadMesh("Cube");
	m_pCommandList->SetPipelineState(m_pResourceManager->LoadPSO("OpaqueLine"));

	for (size_t i = 0; i < m_gridConstants.size(); ++i) {
		CD3DX12_GPU_DESCRIPTOR_HANDLE cd3dMeshGpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cd3dMeshGpuHandle.Offset((UINT)i, m_nCbvDescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(0, cd3dMeshGpuHandle);

		cd3dMeshGpuHandle.Offset(m_nMaterialCBOffset, m_nCbvDescriptorSize);
		m_pCommandList->SetGraphicsRootDescriptorTable(1, cd3dMeshGpuHandle);
		mesh->Render(m_pCommandList.Get());
	}
}

