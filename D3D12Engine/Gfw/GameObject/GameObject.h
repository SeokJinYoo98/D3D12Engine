#pragma once
#include "Gfw/GameComponent/GameComponent.h"

class CGameObject {
public:
	CGameObject();

public:
	void AddMeshComponent(const std::string& meshName, std::shared_ptr<CStaticMeshComponent> pMesh);
	virtual void Update(float fElapsedTime);

	std::shared_ptr<CGameTransform> m_pGameTransform;
protected:
	virtual void UpdateMeshes(float fElapsedTime);

protected:
	std::unordered_map<std::string, std::shared_ptr<CStaticMeshComponent>>	m_pMeshes;
};
