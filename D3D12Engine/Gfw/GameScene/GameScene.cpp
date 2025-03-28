#include "Common\pch.h"
#include "GameScene.h"
#include "Renderer/ResourceManager.h"
#include "Gfw/GamePlayer/GamePlayer.h"

CGameScene::CGameScene()
{
	m_xmf4BgColor = Vector4::StoreFloat4(DirectX::Colors::Black);
}

void CGameScene::OnDestroy()
{
}

void CGameScene::InitScene(UINT nWidth, UINT nHeight)
{
	BuildObjects();
	BuildCameras(nWidth, nHeight);
}
void CGameScene::ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime)
{
	// [ToDo]: 플레이어 입력 처리
	if (m_pGamePlayer) {
		m_pGamePlayer->ProcessInput(pKeysBuffers, fElapsedTime);
	}
}
void CGameScene::Update(float fElapsedTime)
{
	for (auto& pObj : m_pGameObjects)
		pObj->Update(fElapsedTime);
	m_pCamera->Update(fElapsedTime);
}
void CGameScene::OnResize(UINT nWidth, UINT nHeight)
{
	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 45.f);
}

void CGameScene::BuildObjects()
{
	auto CubeMesh = std::make_shared<CStaticMeshComponent>();
	CubeMesh->m_strMeshName = "Cube";
	CubeMesh->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Aquamarine);
	CubeMesh->m_gameTransform.SetPosition(DirectX::XMFLOAT3(-2.f, 0.f, 0.f));

	auto CubeMeshLine = std::make_shared<CStaticMeshComponent>();
	CubeMeshLine->m_strMeshName = "Monkey";
	CubeMeshLine->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::AntiqueWhite);
	CubeMeshLine->m_gameTransform.SetPosition(DirectX::XMFLOAT3(+2.f, 0.f, 0.f));

	m_pGameObjects.resize(2);
	m_pGameObjects[0] = std::make_unique<CGamePlayer>();
	m_pGamePlayer = dynamic_cast<CGamePlayer*>(m_pGameObjects[0].get());
	m_pGamePlayer->m_pGameTransform->SetPosition(0.f, 0.f, -5.f);

	m_pGameObjects[1] = std::make_unique<CGameObject>();
	m_pGameObjects[1]->AddMeshComponent("Cube", CubeMesh);
	m_pGameObjects[1]->AddMeshComponent("CubeLine", CubeMeshLine);

	m_pMeshComps["Opaque"].push_back(CubeMesh);
	m_pMeshComps["OpaqueLine"].push_back(CubeMeshLine);

	auto floor = std::make_shared<CStaticMeshComponent>();
	floor->m_strMeshName = "Cube";
	floor->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::ForestGreen);
	floor->m_gameTransform.SetScale(5.f, 0.1f, 5.f);
	floor->m_gameTransform.SetPosition(0.f, -1.f, 0.f);
	m_pMeshComps["Opaque"].push_back(floor);
}

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();

	OnResize(nWidth, nHeight);

	m_pGamePlayer->SetCamera(m_pCamera);
}
