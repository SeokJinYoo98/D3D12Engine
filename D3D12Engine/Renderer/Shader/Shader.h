#pragma once
#include "Renderer/UploadBuffer/UploadBuffer.h"

class CCamera;

struct CB_Object
{
	DirectX::XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
	DirectX::XMFLOAT4	xmf4Color = { 0.0f, 0.0f, 0.0f, 1.0f };
};

struct CB_Pass {
	DirectX::XMFLOAT4X4 m_xmf4x4View = Matrix4x4::Identity();
	DirectX::XMFLOAT4X4 m_xmf4x4Projection = Matrix4x4::Identity();
};
class CShader 
{
public:
	CShader() = delete;
	CShader(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile);
	virtual ~CShader() { }

	ID3DBlob* GetpShaderBlob() { return m_pShaderBlob.Get(); }
protected:
	void CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile);
	
protected:
	Microsoft::WRL::ComPtr<ID3DBlob>				m_pShaderBlob;
};
class CGameShader
{
public:
	CGameShader();
	virtual ~CGameShader();

public:
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppShaderBlob);

	virtual void CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature, const std::string& psoName);
	virtual void BuildCbvDescriptorHeaps(ID3D12Device* pDevice);
	virtual void BuildConstantBuffers(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, UINT nElementCount);
	virtual void UpdateObjectConstant(CB_Object cbData, UINT cbvIndex);
	virtual void UpdatePassConstant(CB_Pass cbPass);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pCommandList, const DirectX::XMFLOAT4X4& xmf4x4World);
	virtual void ReleaseShaderVariables();
	virtual void UpdatePassCB(ID3D12GraphicsCommandList* pCommandList);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pCommandList, const std::string& psoName, UINT cbIndex);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, const std::string& psoName, UINT cbIndex);

protected:
	// 파이프라인 상태 객체들의 
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>>	m_pPSOs;
	std::unique_ptr<CUploadBuffer<CB_Object>>	m_pObjectCBs;
	std::unique_ptr<CUploadBuffer<CB_Pass>>		m_pPassCBs;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pCbvHeap;
	std::vector<D3D12_INPUT_ELEMENT_DESC>		m_vInputLayouts;

	UINT	m_nCbvDescriptorSize{ };
	UINT currIndex{ };
	UINT passIndex{ };
};

/*
	mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart());

	mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);
		드로우 호출 전에 해야하는데

*/
class CDiffusedShader :public CGameShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppShaderBlob);

	virtual void CreateShader(ID3D12Device* d3dDevice, ID3D12RootSignature* d3dGrapjicsRootSignature, const std::string& psoName);
};
