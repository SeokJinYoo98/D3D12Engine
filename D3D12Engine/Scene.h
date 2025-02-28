#pragma once

class CShader;

class CScene
{
public:
	CScene() = default;
	~CScene();
	
public:
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);
	void ReleaseObjcets();
	void ReleaseUploadBuffers();


	void Processinput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetGraphicsRootSignautre() { return m_pGraphicsRootSignature; }

private:
	std::vector<std::shared_ptr<CShader>> m_pShaders;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature{ nullptr };
};
