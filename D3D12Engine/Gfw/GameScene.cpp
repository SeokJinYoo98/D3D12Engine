#include "Common\pch.h"
#include "GameScene.h"
#include "Renderer/ResourceManager.h"

CGameScene::CGameScene()
{
	m_xmf4BgColor = Vector4::StoreFloat4(DirectX::Colors::White);
	m_xmf3CameraPos = { 0.f, 0.f, -5.f };
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
	if (pKeysBuffers[0x57] & 0x80)  // W key
	{
		m_xmf3CameraPos = Vector3::Add(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::FORWARD, fElapsedTime, false));
	}

	if (pKeysBuffers[0x41] & 0x80)  // A key
	{
		m_xmf3CameraPos = Vector3::Subtract(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::RIGTH, fElapsedTime, false));
	}

	if (pKeysBuffers[0x53] & 0x80)  // S key
	{
		m_xmf3CameraPos = Vector3::Subtract(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::FORWARD, fElapsedTime, false));
	}

	if (pKeysBuffers[0x44] & 0x80)  // D key
	{
		m_xmf3CameraPos = Vector3::Add(m_xmf3CameraPos, Vector3::ScalarProduct(Direction::RIGTH, fElapsedTime, false));
	}

}
void CGameScene::Update(float fElapsedTime)
{
	for (auto& pObj : m_pNewObjects)
		pObj->Update(fElapsedTime);

	DirectX::XMFLOAT3 xmfLookAt = m_xmf3CameraPos;
	xmfLookAt.z += 1.f;
	m_pCamera->GenerateViewMatrix(m_xmf3CameraPos, xmfLookAt, Direction::UP);
}
void CGameScene::OnResize(UINT nWidth, UINT nHeight)
{
	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 45.f);
	m_pCamera->GenerateViewMatrix(m_xmf3CameraPos, Vector3::Normalize(m_xmf3CameraPos), Direction::UP);
}

void CGameScene::BuildObjects()
{
	auto CubeMesh = std::make_shared<CStaticMeshComponent>();
	CubeMesh->m_strMeshName = "Cube";
	CubeMesh->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Aquamarine);
	CubeMesh->Move(DirectX::XMFLOAT3(-1.f, 0.f, -1.f));

	auto CubeMeshLine = std::make_shared<CStaticMeshComponent>();
	CubeMeshLine->m_strMeshName = "Cube";
	CubeMeshLine->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::AntiqueWhite);
	CubeMeshLine->Move(DirectX::XMFLOAT3(+1.f, 0.f, 1.f));

	m_pNewObjects.resize(1);
	m_pNewObjects[0] = std::make_unique<CGameObject>();
	m_pNewObjects[0]->AddMeshComponent(CubeMesh);
	m_pNewObjects[0]->AddMeshComponent(CubeMeshLine);

	m_pMeshComps["Opaque"].push_back(CubeMesh);
	m_pMeshComps["OpaqueLine"].push_back(CubeMeshLine);
}

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();

	OnResize(nWidth, nHeight);
}
