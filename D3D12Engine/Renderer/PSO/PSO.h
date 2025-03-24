#pragma once
class CPSO
{
public:
	CPSO() { }
	virtual ~CPSO() { }

	ID3D12PipelineState*				GetPSO() { return m_pPSO.Get(); }
	virtual void						CreatePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* ppVS, ID3DBlob* ppPS);
protected:
	
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	D3D12_SHADER_BYTECODE				CreateShaderByteCode(ID3DBlob* ppBlob);

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pPSO;
	std::vector<D3D12_INPUT_ELEMENT_DESC>			m_vInputLayouts;
};

class COpaquePSO : public CPSO {
public:
	COpaquePSO() {};
	COpaquePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* ppVS, ID3DBlob* ppPS);
};

class COpaqueLinePSO : public CPSO {
public:
	COpaqueLinePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* ppVS, ID3DBlob* ppPS);

	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
};
