#include "Common\pch.h"
#include "GameScene.h"
#include "Renderer/ResourceManager.h"

CGameScene::CGameScene()
{
	m_xmf4BgColor = Vector4::StoreFloat4(DirectX::Colors::White);
}

void CGameScene::OnDestroy()
{
}

void CGameScene::InitScene(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, UINT nWidth, UINT nHeight)
{
	BuildCameras(nWidth, nHeight);
	BuildObjects(pDevice, pCommandList);
}

void CGameScene::ProcessInput(UCHAR* pKeysBuffers)
{
}

void CGameScene::Update(float fElapsedTime)
{
	//m_pCamera->Update();

	//for (auto& pObj : m_pGameObjects)
	//	pObj->Update(fElapsedTime);
}

void CGameScene::Render(ID3D12GraphicsCommandList* pCommandList)
{
	auto rootSig = CResourceManager::GetInstance().LoadRootSig("Default");
	pCommandList->SetGraphicsRootSignature(rootSig);
	m_pCamera->UpdateShaderVariables(pCommandList);
	m_pCamera->SetViewportsAndScissorRects(pCommandList);

	for (auto& pObj : m_pGameObjects)
		pObj->Render(pCommandList, m_pCamera.get());
}

void CGameScene::OnResize(UINT nWidth, UINT nHeight)
{
	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 45.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CGameScene::ReleaseUploadBuffers()
{
	//for (auto& pObj : m_pGameObjects)
	//	pObj->ReleaseUploadBuffers();
}

void CGameScene::BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	auto CubeMesh = std::make_shared<CStaticMeshComponent>();
	CubeMesh->m_strMeshName = "Cube";
	CubeMesh->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Black);
	CubeMesh->Move(DirectX::XMFLOAT3(-1.f, 0.f, -1.f));

	auto CubeMeshLine = std::make_shared<CStaticMeshComponent>();
	CubeMeshLine->m_strMeshName = "Cube";
	CubeMeshLine->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Black);
	CubeMeshLine->Move(DirectX::XMFLOAT3(+1.f, 0.f, 1.f));

	m_pNewObjects.resize(1);
	m_pNewObjects[0] = std::make_unique<CNewObject>();
	m_pNewObjects[0]->AddMeshComponent(CubeMesh);
	m_pNewObjects[0]->AddMeshComponent(CubeMeshLine);

	m_pMeshCompss.push_back(CubeMesh);
	m_pMeshCompss.push_back(CubeMeshLine);

	//auto& resManager = CResourceManager::GetInstance();
	//auto cubeMesh = resManager.LoadSharedMesh("Cube");


	//m_pGameObjects.resize(3);
	//auto rtObj = std::make_unique<CRotatingObject>();
	//rtObj->SetRotationAxis(Direction::LEFT);
	//rtObj->Move(DirectX::XMFLOAT3(0.f, 0.f, 2.f));
	//rtObj->CbIndex = 0;
	//rtObj->m_xmf4Color = { 1.0f ,0.f, 0.f, 1.f };
	//m_pGameObjects[0] = std::move(rtObj);
	//m_pGameObjects[0]->SetMesh(cubeMesh);

	//auto rtObj2 = std::make_unique<CRotatingObject>();
	//rtObj2->SetRotationAxis(Direction::RIGTH);
	//rtObj2->Move(DirectX::XMFLOAT3(0.f, 0.f, -2.f));
	//rtObj2->CbIndex = 1;
	//rtObj2->m_xmf4Color = { 0.0f , 1.f, 0.f, 1.f };
	//m_pGameObjects[1] = std::move(rtObj2);
	//m_pGameObjects[1]->SetMesh(cubeMesh);


	//auto rtObj3 = std::make_unique<CRotatingObject>();
	//rtObj3->SetRotationAxis(Direction::UP);
	//rtObj3->Move(DirectX::XMFLOAT3(0.f, 1.f, 0.f));
	//rtObj3->CbIndex = 2;
	//rtObj3->m_xmf4Color = { 0.0f , 0.f, 1.f, 1.f };
	//m_pGameObjects[2] = std::move(rtObj3);
	//m_pGameObjects[2]->SetMesh(cubeMesh);


	//auto rootSig = resManager.LoadRootSig("Default");
	//auto pShader = std::make_shared<CDiffusedShader>();
	//pShader->BuildCbvDescriptorHeaps(pDevice);
	//pShader->BuildConstantBuffers(pDevice, pCommandList, static_cast<UINT>(m_pGameObjects.size()));
	//pShader->CreateShader(pDevice, rootSig, "Default");
	//m_pGameObjects[0]->SetShader(pShader);
	//m_pGameObjects[1]->SetShader(pShader);
	//m_pGameObjects[2]->SetShader(pShader);

	//m_pCamera->m_pShader = pShader;
}

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();

	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 45.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}
