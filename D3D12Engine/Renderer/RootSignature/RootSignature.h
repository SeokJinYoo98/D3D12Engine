#pragma once
class CRootSignature
{
public:
	CRootSignature() = default;
	~CRootSignature() { }

	void Init(ID3D12Device* pDevice);
protected:

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
};

