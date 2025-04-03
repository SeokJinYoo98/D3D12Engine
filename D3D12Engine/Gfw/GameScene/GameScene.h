#pragma once
#include "Gfw/GamePlayer/GamePlayer.h"
#include "Gfw/Camera/Camera.h"
#include "Gfw/GameLight/GameLight.h"

class CResourceManager;

class CGameScene {
public:
	CGameScene();
	virtual ~CGameScene() { };
	virtual void OnDestroy();

public:
	void InitScene(UINT nWidth, UINT nHeight, CResourceManager* cResManager);

	virtual void ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime);
	virtual void Update(float fElapsedTime);
	virtual void OnResize(UINT nWidth, UINT nHeight);

	virtual void ReleaseObjcets() { };

	CCamera* GetCamera() { return m_pCamera.get(); }
	const DirectX::XMFLOAT4& GetBgColor() const { return m_xmf4BgColor; }
	const DirectX::XMFLOAT4& GetAmbientLight() const { return m_xmf4AmbientLight; }
	const std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>>& GetMeshes() { return m_pMeshComps; }
	std::vector<CGameDirectionalLight>& GetDirectionalLights() { return m_gameDirectionalLights; }
	std::vector<CGamePointLight>& GetPointLights() { return m_gamePointLights; }
protected:
	virtual void BuildObjects(CResourceManager* cResManager);
	virtual void BuildCameras(UINT nWidth, UINT nHeight);

protected:
	std::shared_ptr<CCamera>					m_pCamera;

	std::vector<CGameDirectionalLight>					m_gameDirectionalLights;
	std::vector<CGamePointLight>						m_gamePointLights;
	std::vector<std::unique_ptr<CGameObject>>			m_pGameObjects;
	std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>> m_pMeshComps;

	DirectX::XMFLOAT4	m_xmf4BgColor;
	DirectX::XMFLOAT4	m_xmf4AmbientLight;
	CGamePlayer*		m_pGamePlayer = nullptr;
};
