#include "Common/pch.h"
#include "GameObject.h"
#include "Gfw/Camera/Camera.h"
CGameObject::CGameObject()
{
	m_pGameTransform = std::make_shared<CGameTransform>();
}
void CGameObject::AddMeshComponent(const std::string& meshName, std::shared_ptr<CStaticMeshComponent> pMesh)
{
	pMesh->m_gameTransform.SetParent(m_pGameTransform);
	m_pMeshes["meshName"] = pMesh;
}
void CGameObject::Update(float fElapsedTime)
{
	UpdateMeshes(fElapsedTime);
}

void CGameObject::UpdateMeshes(float fElapsedTime)
{
}