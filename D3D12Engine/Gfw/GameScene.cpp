#include "Common\pch.h"
#include "GameScene.h"

void CGameScene::OnDestroy()
{
}

void CGameScene::InitScene(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, UINT nWidth, UINT nHeight)
{
	CreateGraphicsRootSignature(pDevice);
	BuildObjects(pDevice, pCommandList);
	BuildCameras(nWidth, nHeight);
}

void CGameScene::ProcessInput(UCHAR* pKeysBuffers)
{
}

void CGameScene::Update(float fElapsedTime)
{
	for (auto& pObj : m_pGameObjects)
		pObj->Animate(fElapsedTime);
}

void CGameScene::Render(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pCamera->UpdateShaderVariables(pCommandList);
	m_pCamera->SetViewportsAndScissorRects(pCommandList);

	for (auto& pObj : m_pGameObjects)
		pObj->Render(pCommandList, m_pCamera.get());
}

void CGameScene::OnResize(UINT nWidth, UINT nHeight)
{
	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 90.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CGameScene::ReleaseUploadBuffers()
{
	for (auto& pObj : m_pGameObjects)
		pObj->ReleaseUploadBuffers();
}

void CGameScene::BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
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

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();
	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 90.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CGameScene::CreateGraphicsRootSignature(ID3D12Device* pDevice)
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
