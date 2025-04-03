#pragma once
class CPSO
{
public:
	CPSO() { }
	virtual ~CPSO() { }

	ID3D12PipelineState*				GetPSO() { return m_pPSO.Get(); }
	virtual void						CreatePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* ppVS, ID3DBlob* ppPS);
protected:
	virtual void						CreateInputLayoutElements();
	
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
private:
	D3D12_SHADER_BYTECODE				CreateShaderByteCode(ID3DBlob* ppBlob);
	D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pPSO;
	std::vector<D3D12_INPUT_ELEMENT_DESC>			m_vInputLayouts;
};

class COpaquePSO : public CPSO {
public:
	COpaquePSO() {};
	COpaquePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* pVS, ID3DBlob* pPS)
	{
		CPSO::CreatePSO(pDevice, pRootSig, pVS, pPS);
	}
};

class COpaqueLinePSO : public CPSO {
public:
	COpaqueLinePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* pVS, ID3DBlob* pPS)
	{
		CPSO::CreatePSO(pDevice, pRootSig, pVS, pPS);
	}
private:
	D3D12_RASTERIZER_DESC		CreateRasterizerState() override;
};

class CDiffusedPSO : public CPSO {
public:
	CDiffusedPSO() {};
	CDiffusedPSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* pVS, ID3DBlob* pPS)
	{
		CPSO::CreatePSO(pDevice, pRootSig, pVS, pPS);
	}

private:
	virtual void CreateInputLayoutElements() override;
};