#include "Common\pch.h"
#include "Scene.h"

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	// 그래픽 루트 시그니쳐 생성
	CreateGraphicsRootSignature(pDevice);

	m_pGameObjects.resize(1);
	auto rtObj = std::make_unique<CRotatingObject>();
	rtObj->SetRotationAxis(Direction::LEFT);
	m_pGameObjects[0] = std::move(rtObj);
	auto pMesh = std::make_shared<CCubeMesh>(pDevice, pCommandList);
	m_pGameObjects[0]->SetMesh(pMesh);

	auto pShader = std::make_shared<CDiffusedShader>();
	pShader->CreateShader(pDevice, m_pGraphicsRootSignature.Get());
	pShader->CreateShaderVariables(pDevice, pCommandList);
	m_pGameObjects[0]->SetShader(pShader);
}

void CScene::ReleaseObjcets()
{
}

void CScene::ReleaseUploadBuffers()
{
	for (auto& pObject : m_pGameObjects) {
		if (pObject) pObject->ReleaseUploadBuffers();
	}
	
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (auto& pObject : m_pGameObjects) {
		if (pObject) pObject->Animate(fTimeElapsed);
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pCommandList, CCamera* camera)
{
	camera->SetViewportsAndScissorRects(pCommandList);
	pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());

	camera->UpdateShaderVariables(pCommandList);

	// 씬을 렌더링 하는 것은 씬을 구성하는 게임 객체들을 렌더링하는 것
	for (auto& pObject : m_pGameObjects) {
		if (pObject) pObject->Render(pCommandList, camera);
	}
}

void CScene::OnResize(int nWidth, int nHeight)
{
}

void CScene::CreateGraphicsRootSignature(ID3D12Device* pDevice)
{
	D3D12_ROOT_PARAMETER pd3dRootParameters[2]{ };
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	//TODO
	HRESULT hResult = pDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_pGraphicsRootSignature.GetAddressOf()));

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
}
