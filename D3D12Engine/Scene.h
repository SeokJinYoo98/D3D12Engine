#pragma once
class CScene
{
public:
	CScene() = default;

	void BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice);
	void ReleaseObjcets();

	void PrepareRender(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& d3dCommandList);
	void Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& d3dCommandList);
	
	void CreateGraphicsRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice);
	void CreateGraphicsPipelineState(const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature{ nullptr };
	// ��Ʈ �ñ״�ó
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState{ nullptr };
	// ���������� ����
};

