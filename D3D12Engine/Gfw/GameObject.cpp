#include "Common/pch.h"
#include "GameObject.h"
#include "Camera.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, DirectX::XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
}

void CGameObject::Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle)
{
	auto mtxRotate = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), DirectX::XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
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

void CGameObject::SetShader(std::shared_ptr<CShader> pShader)
{
	m_pShader = std::move(pShader);
}

void CGameObject::Animate(float fTimeElpased)
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
		m_pShader->UpdateShaderVariable(pCommandList, m_xmf4x4World);
		m_pShader->Render(pCommandList, pCamera);
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

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}
