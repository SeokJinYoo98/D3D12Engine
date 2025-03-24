#pragma once
#include "GameObject.h"
#include "Gfw/Camera/Camera.h"

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

	CCamera*	 GetCamera() { return m_pCamera.get(); }
	const std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>>& GetMeshes() { return m_pMeshComps; }
	const DirectX::XMFLOAT4& GetBgColor() const { return m_xmf4BgColor; }

protected:
	virtual void BuildObjects();
	virtual void BuildCameras(UINT nWidth, UINT nHeight);
private:
	std::unique_ptr<CCamera>					m_pCamera;

	std::vector<std::unique_ptr<CGameObject>> m_pNewObjects;
	std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>> m_pMeshComps;

	DirectX::XMFLOAT4	m_xmf4BgColor;
	DirectX::XMFLOAT3	m_xmf3CameraPos;
};
