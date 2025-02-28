#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"

CScene::~CScene()
{
}

void CScene::BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	// 그래픽 루트 시그니쳐 생성
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pDevice);

	// 씬을 그리기 위한 셰이더 객체 생성
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
	// 루트 시그니처를 파이프라인에 연결한다.
	pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());

	// 씬을 렌더링 하는 것은 씬을 구성하는 셰이더들을 렌더링 하는 것이다.
	for (auto& pShader : m_pShaders) {
		pShader->Render(pCommandList);
	}
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> CScene::CreateGraphicsRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = nullptr;
	//매개변수가 없는 루트 시그너쳐를 생성한다. 
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
