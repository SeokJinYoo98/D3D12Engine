#include "Common\pch.h"
#include "RootSignature.h"

CRootSignature::CRootSignature(ID3D12Device* pDevice)
{
	Init(pDevice);
}

void CRootSignature::Init(ID3D12Device* pDevice)
{
	/*
		셰이더 프로그램은 보통 상수 버퍼, 텍스처, 샘플러와 같이 외부 리소스를 입력으로 사용
		루트 시그니처는 셰이더가 어떤 리소스를 필요로 하는지를 정의하는 역할
		셰이더를 함수에 비유하면, 입력 리소스는 함수의 매개변수와 같고,
		루트 시그니처는 그 함수의 정의를 나타낸다 볼 수 있다.
	*/

	//	루트 파라미터는 디스크립터 테이블, 루트 디스크립터, 루트 상수 등의 형태로 정의될 수 있다.
	// [루트 파라미터 0] : b0을 위한 CBV
	CD3DX12_ROOT_PARAMETER cd3dRootParameters[2];

	CD3DX12_DESCRIPTOR_RANGE rangeObject;
	rangeObject.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE rangePass;
	rangePass.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	cd3dRootParameters[0].InitAsDescriptorTable(1, &rangeObject);
	cd3dRootParameters[1].InitAsDescriptorTable(1, &rangePass);

	// 루트 시그니처 설명자 생성 (두 개의 루트 파라미터를 사용)
	CD3DX12_ROOT_SIGNATURE_DESC cd3dRootSigDesc(
		_countof(cd3dRootParameters), cd3dRootParameters,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);


	Microsoft::WRL::ComPtr<ID3DBlob> pSerializedRootSig;
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

	::D3D12SerializeRootSignature(
		&cd3dRootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		pSerializedRootSig.GetAddressOf(),
		pErrorBlob.GetAddressOf()
	);

	if (pErrorBlob != nullptr)
	{
		::OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
	}

	HRESULT hResult = pDevice->CreateRootSignature(
		0,
		pSerializedRootSig->GetBufferPointer(),
		pSerializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())
	);

	if (FAILED(hResult))
		throw std::runtime_error("Failed to create ");
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
