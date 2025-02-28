#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) 
		m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::SetMesh(std::shared_ptr<CMesh> pMesh)
{
	m_pMesh = pMesh;
}

void CGameObject::SetShader(std::shared_ptr<CShader> pShader)
{
	m_pShader = pShader;
}

void CGameObject::Animate(float fTimeElpased)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	OnPrepareRender();

	if (m_pShader) m_pShader->Render(pCommandList);
	if (m_pMesh) m_pMesh->Render(pCommandList);
}
