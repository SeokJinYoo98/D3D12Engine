#pragma once
#include "Gfw/GameScene/GameScene.h"


// [TODO]: 빛을 추가해보자.
class CMonkey : public CGameObject {
public:
	CMonkey(){ }

};
class CTestScene : public CGameScene
{
protected:
	void BuildObjects() override
	{
		m_pGameObjects.resize(2);
		m_pGameObjects[0] = std::make_unique<CGamePlayer>();
		m_pGamePlayer = dynamic_cast<CGamePlayer*>(m_pGameObjects[0].get());
		m_pGamePlayer->m_pGameTransform->SetPosition(0.f, 0.f, -5.f);

		m_pGameObjects[1] = std::make_unique<CMonkey>();

		auto MonkeyHead = std::make_shared<CStaticMeshComponent>();
		MonkeyHead->m_strMeshName = "Monkey";
		MonkeyHead->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Brown);
		MonkeyHead->m_gameTransform.SetPosition(DirectX::XMFLOAT3(0.f, 1.f, 0.f));
		m_pMeshComps["Opaque"].push_back(MonkeyHead);
		m_pGameObjects[1]->AddMeshComponent("Head", MonkeyHead);

		auto MonkeyBody = std::make_shared<CStaticMeshComponent>();
		MonkeyBody->m_strMeshName = "Cube";
		MonkeyBody->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Brown);
		MonkeyBody->m_gameTransform.SetPosition(DirectX::XMFLOAT3(0.f, -1.f, 0.f));
		MonkeyBody->m_gameTransform.SetScale(0.8f, 1.f, 0.8f);
		m_pMeshComps["Opaque"].push_back(MonkeyBody);
		m_pGameObjects[1]->AddMeshComponent("Body", MonkeyBody);

		auto MonkeyLeftArm = std::make_shared<CStaticMeshComponent>();
		MonkeyLeftArm->m_strMeshName = "Cube";
		MonkeyLeftArm->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Brown);
		MonkeyLeftArm->m_gameTransform.SetPosition(DirectX::XMFLOAT3(-1.f, -1.f, 0.f));
		MonkeyLeftArm->m_gameTransform.SetScale(0.1f, 1.0f, 0.1f);
		m_pMeshComps["Opaque"].push_back(MonkeyLeftArm);
		m_pGameObjects[1]->AddMeshComponent("LeftArm", MonkeyLeftArm);

		auto MonkeyRightArm = std::make_shared<CStaticMeshComponent>();
		MonkeyRightArm->m_strMeshName = "Cube";
		MonkeyRightArm->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Brown);
		MonkeyRightArm->m_gameTransform.SetPosition(DirectX::XMFLOAT3(1.f, -1.f, 0.f));
		MonkeyRightArm->m_gameTransform.SetScale(0.1f, 1.0f, 0.1f);
		m_pMeshComps["Opaque"].push_back(MonkeyRightArm);
		m_pGameObjects[1]->AddMeshComponent("RightArm", MonkeyRightArm);

		auto MonkeyLeftLeg = std::make_shared<CStaticMeshComponent>();
		MonkeyLeftLeg->m_strMeshName = "Cube";
		MonkeyLeftLeg->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Brown);
		MonkeyLeftLeg->m_gameTransform.SetPosition(DirectX::XMFLOAT3(-0.5f, -2.5f, 0.f));
		MonkeyLeftLeg->m_gameTransform.SetScale(0.2f, 1.0f, 0.2f);
		m_pMeshComps["Opaque"].push_back(MonkeyLeftLeg);
		m_pGameObjects[1]->AddMeshComponent("LeftLeg", MonkeyLeftLeg);

		auto MonkeyRightLeg = std::make_shared<CStaticMeshComponent>();
		MonkeyRightLeg->m_strMeshName = "Cube";
		MonkeyRightLeg->m_xmf4Color = Vector4::StoreFloat4(DirectX::Colors::Brown);
		MonkeyRightLeg->m_gameTransform.SetPosition(DirectX::XMFLOAT3(+0.5f, -2.5f, 0.f));
		MonkeyRightLeg->m_gameTransform.SetScale(0.2f, 1.0f, 0.2f);
		m_pMeshComps["Opaque"].push_back(MonkeyRightLeg);
		m_pGameObjects[1]->AddMeshComponent("RightLeg", MonkeyRightLeg);

		m_pGameLights.resize(1);
		m_pGameLights[0] = std::make_unique<CGameLight>();
		m_pGameLights[0]->m_gameTransform.SetPosition(0.f, 10.f, 0.f);
	}
};

