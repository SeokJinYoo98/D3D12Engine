#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"

CScene::~CScene()
{
}

void CScene::BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	// �׷��� ��Ʈ �ñ״��� ����
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pDevice);

	// ���� �׸��� ���� ���̴� ��ü ����
	m_pShaders.reserve(1);

	auto newShader = std::make_shared<CShader>();
	newShader->CreateShader(pDevice, m_pGraphicsRootSignature);
	newShader->BuildObjects(pDevice, pCommandList, nullptr);

	m_pShaders.push_back(newShader);
}

void CScene::ReleaseObjcets()
{
	if (m_pGraphicsRootSignature) m_pGraphicsRootSignature->Release();

	for (auto& pShader : m_pShaders) {
		pShader->ReleaseShaderVariables();
		pShader->ReleaseObjcets();
	}
	m_pShaders.clear();
}

void CScene::ReleaseUploadBuffers()
{
	for (auto& pShader : m_pShaders) 
		pShader->ReleaseUploadBuffers();
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (auto& pShader : m_pShaders) {
		pShader->AnimateObjects(fTimeElapsed);
	}
}

void CScene::Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	// ��Ʈ �ñ״�ó�� ���������ο� �����Ѵ�.
	pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());

	// ���� ������ �ϴ� ���� ���� �����ϴ� ���̴����� ������ �ϴ� ���̴�.
	for (auto& pShader : m_pShaders) {
		pShader->Render(pCommandList);
	}
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> CScene::CreateGraphicsRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = nullptr;
	//�Ű������� ���� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc{ };
	d3dRootSignatureDesc.NumParameters = 0;
	d3dRootSignatureDesc.pParameters = nullptr;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pDevice->CreateRootSignature(
		0, 
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&pd3dGraphicsRootSignature)
	);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	return(pd3dGraphicsRootSignature);
}
