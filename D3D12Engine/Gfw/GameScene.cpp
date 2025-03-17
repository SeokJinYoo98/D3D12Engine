#include "Common\pch.h"
#include "GameScene.h"

void CGameScene::OnDestroy()
{
}

void CGameScene::InitScene(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12DescriptorHeap** pCbvHeap, UINT nWidth, UINT nHeight)
{
	CreateGraphicsRootSignature(pDevice);
	BuildCameras(nWidth, nHeight);
	BuildObjects(pDevice, pCommandList, pCbvHeap);

}

void CGameScene::ProcessInput(UCHAR* pKeysBuffers)
{
}

void CGameScene::Update(float fElapsedTime)
{
	m_pCamera->Update();

	for (auto& pObj : m_pGameObjects)
		pObj->Update(fElapsedTime);
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
	m_pCamera->GenerateProjectionMatrix(1.0f, 100.f, float(nWidth) / float(nHeight), 90.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CGameScene::ReleaseUploadBuffers()
{
	for (auto& pObj : m_pGameObjects)
		pObj->ReleaseUploadBuffers();
}

void CGameScene::BuildObjects(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12DescriptorHeap** pCbvHeap)
{
	m_pGameObjects.resize(2);
	auto rtObj = std::make_unique<CRotatingObject>();
	rtObj->SetRotationAxis(Direction::LEFT);
	rtObj->Move(DirectX::XMFLOAT3(0.f, 0.f, 0.f));
	rtObj->CbIndex = 0;
	m_pGameObjects[0] = std::move(rtObj);
	auto pMesh = std::make_shared<CCubeMesh>(pDevice, pCommandList);
	m_pGameObjects[0]->SetMesh(pMesh);

	auto rtObj2 = std::make_unique<CRotatingObject>();
	rtObj2->SetRotationAxis(Direction::RIGTH);
	rtObj2->Move(DirectX::XMFLOAT3(-2.f, 0.f, 2.f));
	rtObj2->CbIndex = 1;
	m_pGameObjects[1] = std::move(rtObj2);
	auto pMesh2 = std::make_shared<CCubeMesh>(pDevice, pCommandList);
	m_pGameObjects[1]->SetMesh(pMesh2);

	auto pShader = std::make_shared<CDiffusedShader>();
	pShader->BuildCbvDescriptorHeaps(pDevice);
	pShader->BuildConstantBuffers(pDevice, pCommandList, pCbvHeap, static_cast<UINT>(m_pGameObjects.size()));
	pShader->CreateShader(pDevice, m_pGraphicsRootSignature.Get(), "Default");
	m_pGameObjects[0]->SetShader(pShader);
	m_pGameObjects[1]->SetShader(pShader);
	m_pCamera->m_pShader = pShader;
}

void CGameScene::BuildCameras(UINT nWidth, UINT nHeight)
{
	m_pCamera = std::make_unique<CCamera>();

	m_pCamera->SetViewport(0, 0, nWidth, nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, nWidth, nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(nWidth) / float(nHeight), 90.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void CGameScene::CreateGraphicsRootSignature(ID3D12Device* pDevice)
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
		IID_PPV_ARGS(m_pGraphicsRootSignature.GetAddressOf())
	);
}
