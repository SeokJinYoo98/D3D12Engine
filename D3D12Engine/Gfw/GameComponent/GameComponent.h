#pragma once
#include "Common/GameTransform/GameTransform.h"
#include "GameResource/Material/Material.h"
#include "GameResource/Mesh/Mesh.h"

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
	const CMaterial*			GetMat() { return m_pMaterial.get(); }

	CGameTransform			m_gameTransform		= { };
	DirectX::XMFLOAT4			m_xmf4Color;
	std::shared_ptr<CBaseMesh> m_pMesh;
	std::shared_ptr<CMaterial> m_pMaterial;
};

class CPhysicsComponent : public CComponent {
public:
	CPhysicsComponent() {

	}

	float m_fMoveSpeed		= 1.f;
	float m_fRotateSpeed	= 1.f;
};