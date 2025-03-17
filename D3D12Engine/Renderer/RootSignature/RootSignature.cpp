#include "Common\pch.h"
#include "RootSignature.h"

void CRootSignature::Init(ID3D12Device* pDevice)
{
	auto rootSigDesc = CreateRootSignatureDesc();
	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;
	
	if (!SerializeRootSignature(rootSigDesc, &pd3dSignatureBlob, &pd3dErrorBlob)) {
		pd3dSignatureBlob->Release();
		pd3dErrorBlob->Release();
		throw std::runtime_error("Failed to SerializeRootSignature");
	}

	if (!CreateRootSignature(pDevice, pd3dSignatureBlob)) {
		pd3dSignatureBlob->Release();
		pd3dErrorBlob->Release();
		throw std::runtime_error("Failed to CreateRootSignature");
	}

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}

void CRootSignature::Destroy()
{

}

BOOL CRootSignature::CreateRootSignature(ID3D12Device* pDevice, ID3DBlob* pSignatureBlob)
{
/*
	ID3D12RootSignature
	- 셰이더에 전달될 리소스(버퍼, 텍스처, 샘플러 등)를 어떻게 바인딩 할지 정의하는 "설계도"
*/
	HRESULT hResult = pDevice->CreateRootSignature(
		0,
		pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())
	);
	return SUCCEEDED(hResult);
}

BOOL CRootSignature::SerializeRootSignature(const D3D12_ROOT_SIGNATURE_DESC& rootSigDesc, ID3DBlob** pSignatureBlob, ID3DBlob** pErrorBlob)
{
	HRESULT hResult = ::D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		pSignatureBlob,
		pErrorBlob
	);
	return SUCCEEDED(hResult);
}

D3D12_ROOT_SIGNATURE_DESC CRootSignature::CreateRootSignatureDesc()
{

	auto rootParameters = CreateRootParameters();

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters		= static_cast<UINT>(rootParameters.size());
	d3dRootSignatureDesc.pParameters		= rootParameters.data();
	d3dRootSignatureDesc.NumStaticSamplers	= 0;
	d3dRootSignatureDesc.pStaticSamplers	= nullptr;
	d3dRootSignatureDesc.Flags				= CreateRootSignatureFlags();

	return d3dRootSignatureDesc;
}

std::vector<D3D12_ROOT_PARAMETER> CRootSignature::CreateRootParameters()
{
	std::vector<D3D12_ROOT_PARAMETER> d3dRootParameters(2);

	d3dRootParameters[0].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	d3dRootParameters[0].Constants.Num32BitValues	= 16;
	d3dRootParameters[0].Constants.ShaderRegister	= 0;
	d3dRootParameters[0].Constants.RegisterSpace	= 0;
	d3dRootParameters[0].ShaderVisibility			= D3D12_SHADER_VISIBILITY_VERTEX;

	d3dRootParameters[1].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	d3dRootParameters[1].Constants.Num32BitValues	= 32;
	d3dRootParameters[1].Constants.ShaderRegister	= 1;
	d3dRootParameters[1].Constants.RegisterSpace	= 0;
	d3dRootParameters[1].ShaderVisibility			= D3D12_SHADER_VISIBILITY_VERTEX;

	return d3dRootParameters;
}

D3D12_ROOT_SIGNATURE_FLAGS CRootSignature::CreateRootSignatureFlags()
{
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	return d3dRootSignatureFlags;
}
