#include "stdafx.h"
#include "Scene.h"



void CScene::BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice)
{
	CreateGraphicsRootSignature(d3dDevice);
	CreateGraphicsPipelineState(d3dDevice);
}

void CScene::ReleaseObjcets()
{
}

void CScene::PrepareRender(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& d3dCommandList)
{
	// 그래픽 루트 시그니쳐 설정
	d3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	// 파이프 라인 상태 설정
	d3dCommandList->SetPipelineState(m_pPipelineState.Get());
	// 프리미티브 토폴로지를 설정 (삼각형 리스트)
	d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CScene::Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& d3dCommandList)
{
	PrepareRender(d3dCommandList);

	// 정점 3개를 사용해 렌더링한다.
	d3dCommandList->DrawInstanced(3, 1, 0, 0);
}

void CScene::CreateGraphicsRootSignature(const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice)
{
	// 루트 시그니처 생성
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc{ };
	d3dRootSignatureDesc.NumParameters = 0;
	d3dRootSignatureDesc.pParameters = nullptr;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pSignatureBlob{ nullptr };
	ID3DBlob* pErrorBlob{ nullptr };
	::D3D12SerializeRootSignature(
		&d3dRootSignatureDesc, 
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignatureBlob, 
		&pErrorBlob
	);

	d3dDevice->CreateRootSignature(
		0,
		pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_pGraphicsRootSignature)
	);
	if (pSignatureBlob) pSignatureBlob->Release();
	if (pErrorBlob) pErrorBlob->Release();
}

void CScene::CreateGraphicsPipelineState(const Microsoft::WRL::ComPtr<ID3D12Device>& d3dDevice)
{
	// 정점 셰이더 픽셀 셰이더 생성
	ID3DBlob* pVertexShaderBlob{ nullptr };
	ID3DBlob* pPixelShaderBlob{ nullptr };

	UINT nCompileFlags{ 0 };

#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_1", nCompileFlags, 0, &pVertexShaderBlob, nullptr);
	D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_1", nCompileFlags, 0, &pPixelShaderBlob, nullptr);

	// 래스터라이저 상태 설정
	D3D12_RASTERIZER_DESC d3dRasterizerDesc{ };
	d3dRasterizerDesc.FillMode			= D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode			= D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// 블렌드 상태 설정
	D3D12_BLEND_DESC d3dBlendDesc{ };
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// 그래픽스 파이프라인 상태를 설정
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc{ };
	d3dPipelineStateDesc.pRootSignature = m_pGraphicsRootSignature.Get();
	d3dPipelineStateDesc.VS.pShaderBytecode = pVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	d3dDevice->CreateGraphicsPipelineState(
		&d3dPipelineStateDesc,
		IID_PPV_ARGS(&m_pPipelineState)
	);
	
	if (pVertexShaderBlob) pVertexShaderBlob->Release();
	if (pPixelShaderBlob) pPixelShaderBlob->Release();
}
