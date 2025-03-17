#pragma once
class CRootSignature
{
public:
	CRootSignature() { };
	virtual ~CRootSignature() { }

	void					Init(ID3D12Device* pDevice);
	virtual void			Destroy();

	ID3D12RootSignature*	GetRootSignature() { return m_pRootSignature.Get(); }
protected:
	virtual BOOL								CreateRootSignature(ID3D12Device* pDevice, ID3DBlob* pSignatureBlob);
	virtual BOOL								SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC& rootSigDesc, ID3DBlob** pSignatureBlob, ID3DBlob** pErrorBlob);
	virtual D3D12_ROOT_SIGNATURE_DESC			CreateRootSignatureDesc();
	virtual std::vector<D3D12_ROOT_PARAMETER>	CreateRootParameters();
	virtual D3D12_ROOT_SIGNATURE_FLAGS			CreateRootSignatureFlags();
private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
};

