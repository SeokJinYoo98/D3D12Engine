#include "Common\pch.h"
#include "GameScene.h"
#include "GameResource/ResourceManager.h"
#include "Gfw/GamePlayer/GamePlayer.h"

CGameScene::CGameScene()
	:m_xmf4AmbientLight{ 0.1f, 0.1f, 0.1f, 1.0f }
{
	m_xmf4BgColor = Vector4::StoreFloat4(DirectX::Colors::Black);
}

void CGameScene::OnDestroy()
{
}

void CGameScene::InitScene(UINT nWidth, UINT nHeight, CResourceManager* cResManager)
{
	BuildObjects(cResManager);
	BuildCameras(nWidth, nHeight);
}
void CGameScene::ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime)
{
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

void CGameScene::BuildObjects(CResourceManager* cResManager)
{

}

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();
	OnResize(nWidth, nHeight);
	m_pGamePlayer->SetCamera(m_pCamera);
}
