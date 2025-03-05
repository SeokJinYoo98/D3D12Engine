#pragma once
#include "GameObject.h"
#include "Camera.h"

class CScene
{
public:
	CScene() = default;
	~CScene();
	
public:
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void ReleaseObjcets();
	void ReleaseUploadBuffers();

	void ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pCommandList, CCamera* camera);
	void OnResize(int nWidth, int nHeight);

	void CreateGraphicsRootSignature(ID3D12Device* pDevice);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetGraphicsRootSignautre() { return m_pGraphicsRootSignature; }
private:
	// ���� ���� ��ü���� ����. ���� ��ü�� ���̴��� �����Ѵ�.
	std::vector<std::unique_ptr<CGameObject>> m_pGameObjects;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;
};
