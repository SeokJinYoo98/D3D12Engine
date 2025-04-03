#pragma once
#include "Gfw/GameObject/GameObject.h"
#include "Common/GameTransform/GameTransform.h"

struct DirectionLight {
	DirectX::XMFLOAT3 xmf3Direction;
	float fpadding1 = 0.f;
	DirectX::XMFLOAT3 xmf3Strength;
	float fpadding2 = 0.f;
};

struct PointLight {
	DirectX::XMFLOAT3	xmf3Strength;
	float				fFallOffStart;
	DirectX::XMFLOAT3	xmf3Position;
	float				fFallOffEnd;
};

class CGameDirectionalLight {
public:
	CGameDirectionalLight() {
		m_pGameTransform = std::make_shared<CGameTransform>();
		m_xmf3Strength = { 0.6f, 0.6f, 0.6f };
	};
	const DirectionLight GetLightData() {
		DirectionLight data;
		data.xmf3Direction = m_pGameTransform->GetDownVector();
		data.xmf3Strength = m_xmf3Strength;
		return data;
	}
	void AddMesh(std::shared_ptr<CStaticMeshComponent> pMesh) {
		m_pMesh = pMesh;
		m_pMesh->m_gameTransform.SetParent(m_pGameTransform);
	}
	std::shared_ptr<CGameTransform> m_pGameTransform;
protected:
	std::shared_ptr<CStaticMeshComponent> m_pMesh;
	DirectX::XMFLOAT3	m_xmf3Strength;
};

// -------------------

class CGamePointLight : public CGameDirectionalLight {
public:
	CGamePointLight() {
		m_pGameTransform->Translate(0.f, 0.f, 5.f);
		m_fFallOffStart = 1.0f;
		m_fFallOffEnd = 100.f;
		m_xmf3Strength = { 1.f, 1.f, 1.f };
	}
	PointLight GetPointLightData() {
		PointLight data;
		data.xmf3Position = m_pGameTransform->GetPosition();
		data.fFallOffStart = m_fFallOffStart;
		data.xmf3Strength = m_xmf3Strength;
		data.fFallOffEnd = m_fFallOffEnd;
		return data;
	}
protected:
	float m_fFallOffStart;
	float m_fFallOffEnd;
};

