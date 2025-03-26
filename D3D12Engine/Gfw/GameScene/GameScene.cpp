#include "Common\pch.h"
#include "GameScene.h"
#include "Renderer/ResourceManager.h"

CGameScene::CGameScene()
{
	m_xmf4BgColor = Vector4::StoreFloat4(DirectX::Colors::Black);
	m_xmf3CameraPos = { 0.f, 0.f, -5.f };
	m_xmf3CameraLookAt = { 0.f, 0.f, 0.f };
}

void CGameScene::OnDestroy()
{
}

void CGameScene::InitScene(UINT nWidth, UINT nHeight)
{
	BuildCameras(nWidth, nHeight);
	BuildObjects();
}
void CGameScene::ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime)
{
	float fMvSpeed = 10.f * fElapsedTime;
	float fRtSpeed = 1.f * fElapsedTime;

	if (pKeysBuffers[0x57] & 0x80)  // W key
	{
		m_xmf3CameraPos = Vector3::Add(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::FORWARD, fMvSpeed, false));
	}

	if (pKeysBuffers[0x41] & 0x80)  // A key
	{
		m_xmf3CameraPos = Vector3::Subtract(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::RIGTH, fMvSpeed, false));
	}

	if (pKeysBuffers[0x53] & 0x80)  // S key
	{
		m_xmf3CameraPos = Vector3::Subtract(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::FORWARD, fMvSpeed, false));
	}

	if (pKeysBuffers[0x44] & 0x80)  // D key
	{
		m_xmf3CameraPos = Vector3::Add(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::RIGTH, fMvSpeed, false));
	}
	if (pKeysBuffers[0x51] & 0x80)  // Q key
	{
		m_xmf3CameraPos = Vector3::RotateY(m_xmf3CameraPos, fRtSpeed);
	}
	if (pKeysBuffers[0x45] & 0x80)  // E key
	{
		m_xmf3CameraPos = Vector3::RotateY(m_xmf3CameraPos, -fRtSpeed);
	}
}
void CGameScene::Update(float fElapsedTime)
{
	for (auto& pObj : m_pGameObjects)
		pObj->Update(fElapsedTime);
	m_pCamera->GenerateViewMatrix(m_xmf3CameraPos, m_xmf3CameraLookAt, Direction::UP);
}
void CGameScene::OnResize(UINT nWidth, UINT nHeight)
{
	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 45.f);
	m_pCamera->GenerateViewMatrix(m_xmf3CameraPos, m_xmf3CameraLookAt, Direction::UP);
}

void CGameScene::BuildObjects()
{
	auto CubeMesh = std::make_shared<CStaticMeshComponent>();
	CubeMesh->m_strMeshName = "Cube";
	CubeMesh->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Aquamarine);
	CubeMesh->Move(DirectX::XMFLOAT3(-2.f, 0.f, 0.f));

	auto CubeMeshLine = std::make_shared<CStaticMeshComponent>();
	CubeMeshLine->m_strMeshName = "Tank";
	CubeMeshLine->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::AntiqueWhite);
	CubeMeshLine->Move(DirectX::XMFLOAT3(+2.f, 0.f, 0.f));

	m_pGameObjects.resize(1);
	m_pGameObjects[0] = std::make_unique<CGameObject>();
	m_pGameObjects[0]->AddMeshComponent(CubeMesh);
	m_pGameObjects[0]->AddMeshComponent(CubeMeshLine);

	m_pMeshComps["Opaque"].push_back(CubeMesh);
	m_pMeshComps["Opaque"].push_back(CubeMeshLine);

	auto floor = std::make_shared<CStaticMeshComponent>();
	floor->m_strMeshName = "Cube";
	floor->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::ForestGreen);
	floor->Scale(5.f, 0.1f, 5.f);
	floor->Move(0.f, -10.f, 0.f);
	m_pMeshComps["Opaque"].push_back(floor);
}

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();

	OnResize(nWidth, nHeight);
}
