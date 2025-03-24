#include "Common/pch.h"
#include "GameObject.h"
#include "Gfw/Camera/Camera.h"

void CGameObject::AddMeshComponent(std::shared_ptr<CStaticMeshComponent> pMesh)
{
	pMesh->m_pWorld = &m_xmf4x4World;
	m_pMeshes.push_back(pMesh);
}

CGameObject::CGameObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
	Scale(1.0f, 1.f, 1.0f);
}

void CGameObject::Update(float fElapsedTime)
{
	Rotate(1.f, 0.f, 0.f, 45.f * fElapsedTime);
	UpdateMeshes(fElapsedTime);
}

void CGameObject::UpdateMeshes(float fElapsedTime)
{
	for (size_t i = 0; i < m_pMeshes.size(); ++i) {
		if (i == 0)
			m_pMeshes[i]->Rotate(0.f, 0.f, 1.f, 120.f * fElapsedTime);
		else
			m_pMeshes[i]->Rotate(-1.f, 0.f, 0.f, 120.f * fElapsedTime);
	}
}

void CGameObject::Rotate(float x, float y, float z, float fAngle)
{
	Rotate(DirectX::XMFLOAT3(x, y, z), fAngle);
}

void CGameObject::Move(float x, float y, float z)
{
	Move(DirectX::XMFLOAT3(x, y, z));
}

void CGameObject::Scale(float x, float y, float z)
{
	Scale(DirectX::XMFLOAT3(x, y, z));
}

void CGameObject::Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle)
{
	auto mtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), DirectX::XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(const DirectX::XMFLOAT3& xmf3Pos)
{
	auto translation = DirectX::XMMatrixTranslation(xmf3Pos.x, xmf3Pos.y, xmf3Pos.z);
	m_xmf4x4World = Matrix4x4::Multiply(translation, m_xmf4x4World);
}

void CGameObject::Scale(const DirectX::XMFLOAT3& xmf3Scale)
{
	auto scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	m_xmf4x4World = Matrix4x4::Multiply(scale, m_xmf4x4World);
}
