#pragma once
#include "GameObject.h"
#include "Gfw/Camera/Camera.h"

class CGameScene {
public:
	CGameScene() = default;
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

protected:
	virtual void BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void BuildCameras(UINT nWidth, UINT nHeight);
	virtual void CreateGraphicsRootSignature(ID3D12Device* pDevice);

private:
	std::vector<std::unique_ptr<CGameObject>>	m_pGameObjects;
	std::unique_ptr<CCamera>					m_pCamera;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;
};
