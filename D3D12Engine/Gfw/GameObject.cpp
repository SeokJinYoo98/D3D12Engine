#include "Common/pch.h"
#include "GameObject.h"
#include "Gfw/Camera/Camera.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, DirectX::XMMatrixIdentity());
	Scale(0.5f, 1.0f, 1.0f);
}

CGameObject::~CGameObject()
{
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
	XMStoreFloat4x4(&m_xmf4x4World, translation);
}

void CGameObject::Scale(const DirectX::XMFLOAT3& xmf3Scale)
{
	auto scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	XMStoreFloat4x4(&m_xmf4x4World, scale);
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) 
		m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::SetMesh(std::shared_ptr<CMesh> pMesh)
{
	m_pMesh = std::move(pMesh);
}

void CGameObject::SetShader(std::shared_ptr<CGameShader> pShader)
{
	m_pShader = std::move(pShader);
}

void CGameObject::Update(float fTimeElpased)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	// 게임 객체의 월드 변환 행렬을 셰이더의 상수 버퍼로 전달한다.
	if (m_pShader) {
		m_pShader->Render(pCommandList, "Default", CbIndex);
	}

	if (m_pMesh) m_pMesh->Render(pCommandList);
}

CRotatingObject::CRotatingObject()
	:m_xmf3RotationAxis{ DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, m_fRotationSpeed{ 90.f }
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Update(float fTimeElapsed)
{
	CGameObject::Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
	CB_Object myCb;
	myCb.xmf4x4World = m_xmf4x4World;
	myCb.xmf4Color = m_xmf4Color;

	m_pShader->UpdateObjectConstant(myCb, CbIndex);
}

void CNewObject::AddMeshComponent(std::shared_ptr<CStaticMeshComponent> pMesh)
{
	pMesh->m_pWorld = &m_xmf4x4World;
	m_pMeshes.push_back(pMesh);
}

CNewObject::CNewObject()
{
	m_xmf4x4World = Matrix4x4::Identity();
	Scale(0.5f, 1.f, 0.5f);
}

void CNewObject::Rotate(float x, float y, float z, float fAngle)
{
	Rotate(DirectX::XMFLOAT3(x, y, z), fAngle);
}

void CNewObject::Move(float x, float y, float z)
{
	Move(DirectX::XMFLOAT3(x, y, z));
}

void CNewObject::Scale(float x, float y, float z)
{
	Scale(DirectX::XMFLOAT3(x, y, z));
}

void CNewObject::Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle)
{
	auto mtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), DirectX::XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CNewObject::Move(const DirectX::XMFLOAT3& xmf3Pos)
{
	auto translation = DirectX::XMMatrixTranslation(xmf3Pos.x, xmf3Pos.y, xmf3Pos.z);
	m_xmf4x4World = Matrix4x4::Multiply(translation, m_xmf4x4World);
}

void CNewObject::Scale(const DirectX::XMFLOAT3& xmf3Scale)
{
	auto scale = DirectX::XMMatrixScaling(xmf3Scale.x, xmf3Scale.y, xmf3Scale.z);
	m_xmf4x4World = Matrix4x4::Multiply(scale, m_xmf4x4World);
}
