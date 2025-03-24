#include "Common\pch.h"
#include "Component.h"

CStaticMeshComponent::CStaticMeshComponent()
{
	m_xmf4x4Local = Matrix4x4::Identity();
}

DirectX::XMFLOAT4X4 CStaticMeshComponent::GetTransform() const
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = m_xmf4x4Local;

	if (m_pWorld) {
		xmf4x4Tr = Matrix4x4::Multiply(xmf4x4Tr, *m_pWorld);
	}

	return xmf4x4Tr;
}

const DirectX::XMFLOAT4& CStaticMeshComponent::GetColor() const
{
	return m_xmf4Color;
}

void CStaticMeshComponent::Rotate(float x, float y, float z, float fAngle)
{
	Rotate(DirectX::XMFLOAT3(x, y, z), fAngle);
}

void CStaticMeshComponent::Move(float x, float y, float z)
{
	Move(DirectX::XMFLOAT3(x, y, z));
}

void CStaticMeshComponent::Scale(float x, float y, float z)
{
	Scale(DirectX::XMFLOAT3(x, y, z));
}

void CStaticMeshComponent::Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle)
{
	auto mtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), DirectX::XMConvertToRadians(fAngle));
	m_xmf4x4Local = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Local);
}

void CStaticMeshComponent::Move(const DirectX::XMFLOAT3& xmf3Pos)
{
	auto translation = DirectX::XMMatrixTranslation(xmf3Pos.x, xmf3Pos.y, xmf3Pos.z);
	m_xmf4x4Local = Matrix4x4::Multiply(translation, m_xmf4x4Local);
}

void CStaticMeshComponent::Scale(const DirectX::XMFLOAT3& xmf3Scale)
{
	auto scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	m_xmf4x4Local = Matrix4x4::Multiply(scale, m_xmf4x4Local);
}
