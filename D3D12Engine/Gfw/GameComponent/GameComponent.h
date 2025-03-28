#pragma once
#include "Common/GameTransform/GameTransform.h"

class CComponent
{
public: 
	CComponent() = default;
	virtual void Update(float fElasedTime){ }
};

class CStaticMeshComponent : public CComponent {
public:
	CStaticMeshComponent();
	DirectX::XMFLOAT4X4			GetTransform();
	const DirectX::XMFLOAT4&	GetColor() const;

	CGameTransform			m_gameTransform		= { };
	DirectX::XMFLOAT4		m_xmf4Color			= { 0.f, 0.f, 0.f, 1.f };
	std::string				m_strMeshName		= { "None" };
};

class CPhysicsComponent : public CComponent {
public:
	CPhysicsComponent() {

	}

	float m_fMoveSpeed		= 1.f;
	float m_fRotateSpeed	= 1.f;
};