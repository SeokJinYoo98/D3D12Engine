#pragma once
#include "GameObject.h"
#include "Gfw/Camera/Camera.h"

class CGameScene {
public:
	CGameScene();
	virtual ~CGameScene() { };
	virtual void OnDestroy();

public:
	void InitScene(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, UINT nWidth, UINT nHeight);

	virtual void ProcessInput(UCHAR* pKeysBuffers);
	virtual void Update(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);
	virtual void OnResize(UINT nWidth, UINT nHeight);

	virtual void ReleaseObjcets() { };
	virtual void ReleaseUploadBuffers();

	CCamera*	 GetCamera() { return m_pCamera.get(); }
	const std::vector<std::shared_ptr<CStaticMeshComponent>>& GetMeshComps() { return m_pMeshCompss; }
	const std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>>& GetMeshes() { return m_pMeshComps; }
	const DirectX::XMFLOAT4& GetBgColor() const { return m_xmf4BgColor; }
protected:
	virtual void BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void BuildCameras(UINT nWidth, UINT nHeight);
private:
	std::vector<std::unique_ptr<CGameObject>>	m_pGameObjects;
	std::unique_ptr<CCamera>					m_pCamera;

	std::vector<std::unique_ptr<CNewObject>> m_pNewObjects;
	std::vector<std::shared_ptr<CStaticMeshComponent>> m_pMeshCompss;

	std::unordered_map<std::string, std::vector<std::shared_ptr<CStaticMeshComponent>>> m_pMeshComps;
	DirectX::XMFLOAT4 m_xmf4BgColor;
};
