#pragma once
#include "Gfw/GameObject.h"
class CResourceManager
{
public:
	CResourceManager() = default;
	void Init();

private:
	std::unordered_map<std::string, std::shared_ptr<CShader>>		m_gameShaderMap;
	std::unordered_map<std::string, std::shared_ptr<CGameObject>>	m_gameObjectMap;
	std::unordered_map<std::string, std::shared_ptr<CMesh>>			m_gameMeshMap;
};

