#include "Common\pch.h"
#include "GameTransform.h"

CGameTransform::CGameTransform()
	:m_xmf3Position{0.f, 0.f, 0.f,}, m_xmf3Rotation{0.f, 0.f, 0.f}, m_xmf3Scale{1.f, 1.f, 1.f}
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	UpdateTransform();
}

DirectX::XMFLOAT4X4 CGameTransform::GetTransform()
{
	UpdateTransform();
	if (m_pParentTransform) {
		return Matrix4x4::Multiply(m_xmf4x4Transform, m_pParentTransform->GetTransform());
	}
	return m_xmf4x4Transform;
}

DirectX::XMFLOAT3 CGameTransform::GetPosition()
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = GetTransform();
	
	return Matrix4x4::GetPosition(xmf4x4Tr);
}

DirectX::XMFLOAT3 CGameTransform::GetForwardVector()
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = GetTransform();
	return Matrix4x4::GetForwardVector(xmf4x4Tr);
}

DirectX::XMFLOAT3 CGameTransform::GetBackwardVector()
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = GetTransform();
	return Vector3::ScalarProduct(Matrix4x4::GetForwardVector(xmf4x4Tr), -1.f);
}

DirectX::XMFLOAT3 CGameTransform::GetRightVector()
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = GetTransform();
	return Matrix4x4::GetRightVector(xmf4x4Tr);
}

DirectX::XMFLOAT3 CGameTransform::GetLeftVector()
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = GetTransform();
	return Vector3::ScalarProduct(Matrix4x4::GetRightVector(xmf4x4Tr), -1.f);
}

DirectX::XMFLOAT3 CGameTransform::GetPosPlusForward()
{
	DirectX::XMFLOAT4X4 xmf4x4Tr = GetTransform();
	return Vector3::Add(Matrix4x4::GetPosition(xmf4x4Tr), Matrix4x4::GetForwardVector(xmf4x4Tr));
}

void CGameTransform::SetScaleX(float fScaleX)
{
	m_xmf3Scale.x = fScaleX;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetScaleY(float fScaleY)
{
	m_xmf3Scale.y = fScaleY;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetScaleZ(float fScaleZ)
{
	m_xmf3Scale.z = fScaleZ;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetScale(float fScaleX, float fScaleY, float fScaleZ)
{
	SetScale(DirectX::XMFLOAT3(fScaleX, fScaleY, fScaleZ));
}

void CGameTransform::SetScale(const DirectX::XMFLOAT3& xmf3Scale)
{
	m_xmf3Scale = xmf3Scale;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetPositionX(float fPosX)
{
	m_xmf3Position.x = fPosX;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetPositionY(float fPosY)
{
	m_xmf3Position.y = fPosY;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetPositionZ(float fPosZ)
{
	m_xmf3Position.z = fPosZ;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetPosition(float fPosX, float fPosY, float fPosZ)
{
	SetPosition(DirectX::XMFLOAT3(fPosX, fPosY, fPosZ));
}

void CGameTransform::SetPosition(const DirectX::XMFLOAT3& xmf3Position)
{
	m_xmf3Position = xmf3Position;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetRotationYaw(float fYaw)
{
	m_xmf3Rotation.y = fYaw;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetRotationPitch(float fPitch)
{
	m_xmf3Rotation.x = fPitch;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetRotationRoll(float fRoll)
{
	m_xmf3Rotation.z = fRoll;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::SetRotation(float fYaw, float fPitch, float fRoll)
{
	SetRotation(DirectX::XMFLOAT3(fYaw, fPitch, fRoll));
}

void CGameTransform::SetRotation(const DirectX::XMFLOAT3& xmf3Roatation)
{
	m_xmf3Rotation = xmf3Roatation;
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::Translate(float x, float y, float z)
{
	Translate(DirectX::XMFLOAT3(x, y, z));
}

void CGameTransform::Translate(const DirectX::XMFLOAT3& xmf3DeltaPosition)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3DeltaPosition);
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::Rotate(float x, float y, float z)
{
	Rotate(DirectX::XMFLOAT3(x, y, z));
}

void CGameTransform::Rotate(const DirectX::XMFLOAT3& xmf3DeltaRotate)
{
	m_xmf3Rotation = Vector3::Add(m_xmf3Rotation, xmf3DeltaRotate);
	std::cout << m_xmf3Rotation.x << ", " << m_xmf3Rotation.y << ", " << m_xmf3Rotation.z << '\n';
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::Scale(float x, float y, float z)
{
	Scale(DirectX::XMFLOAT3(x, y, z));
}

void CGameTransform::Scale(const DirectX::XMFLOAT3& xmf3DeltaScale)
{
	m_xmf3Scale = Vector3::Add(m_xmf3Scale, xmf3DeltaScale);
	if (!m_bDirtyTransform)
		m_bDirtyTransform = true;
}

void CGameTransform::UpdateTransform()
{
	if (!m_bDirtyTransform) return;

	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_xmf3Rotation.x, m_xmf3Rotation.y, m_xmf3Rotation.z);
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z);

	DirectX::XMMATRIX transform = scale * rotation * translation;

	DirectX::XMStoreFloat4x4(&m_xmf4x4Transform, transform);

	m_bDirtyTransform = false;
}
