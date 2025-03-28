#pragma once
#include "Gfw/GamePlayer/GamePlayer.h"
#include "Gfw/Camera/Camera.h"

class CGamePlayer;

class CGameScene {
public:
	CGameScene();
	virtual ~CGameScene() { };
	virtual void OnDestroy();

public:
	void InitScene(UINT nWidth, UINT nHeight);

	virtual void ProcessInput(UCHAR* pKeysBuffers, float fElapsedTime);
	virtual void Update(float fElapsedTime);
	virtual void OnResize(UINT nWidth, UINT nHeight);

	virtual void ReleaseObjcets() { };

	const DirectX::XMFLOAT4& GetBgColor() const { return m_xmf4BgColor; }
	CCamera*	 GetCamera() { return m_pCamera.get(); }
	const std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>>& GetMeshes() { return m_pMeshComps; }
	
protected:
	virtual void BuildObjects();
	virtual void BuildCameras(UINT nWidth, UINT nHeight);

protected:
	std::shared_ptr<CCamera>					m_pCamera;

	std::vector<std::unique_ptr<CGameObject>> m_pGameObjects;
	std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>> m_pMeshComps;

	DirectX::XMFLOAT4	m_xmf4BgColor;
	CGamePlayer*		m_pGamePlayer = nullptr;
};
