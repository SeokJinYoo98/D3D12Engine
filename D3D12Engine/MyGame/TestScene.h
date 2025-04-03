#pragma once
#include "Gfw/GameScene/GameScene.h"
#include "GameResource/ResourceManager.h"

// 조명 완성
// 머터리얼, 텍스처 입히기
// 태양계 완성
// 레벨 에디터
class CMonkey : public CGameObject {
public:
	CMonkey(){ }

};
class CTestScene : public CGameScene
{
public:
	CTestScene() {
		m_xmf4BgColor = Vector4::StoreFloat4(DirectX::Colors::SkyBlue);
		m_xmf4AmbientLight = { 0.2f, 0.2f, 0.2f, 1.0f };
	}

	void Update(float fElap) override {
		CGameScene::Update(fElap);
		for (auto& light : m_gamePointLights) {
			// 1. 현재 위치 가져오기
			DirectX::XMFLOAT3 pos = light.m_pGameTransform->GetPosition();

			// 2. Y축 기준 회전 행렬 생성 (라디안 단위)
			float radAngle = DirectX::XMConvertToRadians(10.f * fElap);
			DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(radAngle);

			// 3. 위치 벡터를 회전 적용
			DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
			posVec = DirectX::XMVector3Transform(posVec, rotationMatrix);

			// 4. 회전된 위치로 업데이트
			DirectX::XMFLOAT3 newPos;
			DirectX::XMStoreFloat3(&newPos, posVec);
			light.m_pGameTransform->SetPosition(newPos);
		}
	}
protected:
	void BuildObjects(CResourceManager* pResManager) override
	{
		m_pGameObjects.resize(2);
		m_pGameObjects[0] = std::make_unique<CGamePlayer>();
		m_pGamePlayer = dynamic_cast<CGamePlayer*>(m_pGameObjects[0].get());
		m_pGamePlayer->m_pGameTransform->SetPosition(0.f, 0.f, -5.f);

		auto Cube1 = std::make_shared<CStaticMeshComponent>();
		Cube1->m_pMesh = pResManager->LoadSharedMesh("Monkey");
		Cube1->m_pMaterial = pResManager->LoadMat("Red");
		Cube1->m_gameTransform.Translate(-5.f, 0.f, 0.f);
		Cube1->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Red);
		m_pMeshComps["Opaque"].push_back(Cube1);

		auto Cube2 = std::make_shared<CStaticMeshComponent>();
		Cube2->m_pMesh = pResManager->LoadSharedMesh("Sphere");
		Cube2->m_pMaterial = pResManager->LoadMat("Blue");
		Cube2->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Blue);
		m_pMeshComps["Opaque"].push_back(Cube2);

		auto Cube3 = std::make_shared<CStaticMeshComponent>();
		Cube3->m_pMesh = pResManager->LoadSharedMesh("Cylinder");
		Cube3->m_pMaterial = pResManager->LoadMat("Green");
		Cube3->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Green);
		Cube3->m_gameTransform.Translate(5.f, 0.f, 0.f);
		m_pMeshComps["Opaque"].push_back(Cube3);


		m_pGameObjects[1] = std::make_unique<CGameObject>();
		m_pGameObjects[1]->AddMeshComponent("Cube1", Cube1);
		m_pGameObjects[1]->AddMeshComponent("Cube2", Cube2);
		m_pGameObjects[1]->AddMeshComponent("Cube3", Cube3);

		m_gameDirectionalLights.resize(1);
		m_gamePointLights.resize(1);

		auto floor = std::make_shared<CStaticMeshComponent>();
		floor->m_pMesh = pResManager->LoadSharedMesh("Cube");
		floor->m_pMaterial = pResManager->LoadMat("Green");
		floor->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Green);
		floor->m_gameTransform.SetScale(10.f, 0.1f, 10.f);
		floor->m_gameTransform.SetPosition(0.f, -0.9f, 0.f);
		m_pMeshComps["Opaque"].push_back(floor);
	}
};

