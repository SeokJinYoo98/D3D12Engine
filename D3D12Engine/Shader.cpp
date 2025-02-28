#include "stdafx.h"
#include "Shader.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	if (m_ppPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; ++i)
			if (m_ppPipelineStates[i])
				m_ppPipelineStates[i]->Release();
		delete[] m_ppPipelineStates;
	}
}

// �����Ͷ����� ���¸� �����ϱ� ���� ����ü�� ��ȯ
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc{ };
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3dRasterizerDesc;
}

// Depth-Stencil �˻縦 ���� ���¸� �����ϱ� ���� ����ü
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc{ };
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
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

	return d3dBlendDesc;
}

// �Է� �����⿡�� ���� ������ ������ �˷��ֱ� ���� ����ü�� ��ȯ.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	UINT nInputElementsDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementsDescs];

	pInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{ };
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementsDescs;

	return d3dInputLayoutDesc;
}

// ���� ���̴� ����Ʈ �ڵ带 �����Ѵ�.
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppShaderBlob)
{
	return(CompileShaderFromFile(L"Shaders.hlsl", "VSMain", "vs_5_1", ppShaderBlob));
}
// �ȼ� ���̴� ����Ʈ �ڵ带 �����Ѵ�.
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", "PSMain", "ps_5_1", ppShaderBlob);
}
// ���̴� �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü�� ��ȯ�Ѵ�.
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppShaderBlob)
{
	UINT nComPileFlags{ };
#if defined(_DEBUG)
	nComPileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DCompileFromFile(
		pszFileName, nullptr, nullptr,
		pszShaderName, pszShaderProfile,
		nComPileFlags, 0, ppShaderBlob, nullptr
	);

	D3D12_SHADER_BYTECODE d3dShaderByteCode{ };
	d3dShaderByteCode.BytecodeLength = (*ppShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

// �׷��Ƚ� ���������� ���� ��ü�� �����Ѵ�.
void CShader::CreateShader(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& pRootSignature)
{
	// �׷��Ƚ� ���������� ���� ��ü �迭�� ����
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	ID3DBlob* pVertexShaderBlob = nullptr, * pPixelShaderBlob = nullptr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc{ };
	d3dPipelineStateDesc.pRootSignature = pRootSignature.Get();
	d3dPipelineStateDesc.VS = CreateVertexShader(&pVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	pDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, IID_PPV_ARGS(&m_ppPipelineStates[0]));

	if (pVertexShaderBlob) pVertexShaderBlob->Release();
	if (pPixelShaderBlob) pPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader::ReleaseUploadBuffers()
{
	for (auto& pObject : m_pObjects)
		pObject->ReleaseUploadBuffers();
}

void CShader::BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList, void* pConstext)
{
	auto pTriangleMesh = std::make_shared<CTriangleMesh>(pDevice, pCommandList);

	auto pNewObject = std::make_shared<CGameObject>();
	pNewObject->SetMesh(pTriangleMesh);

	m_pObjects.push_back(pNewObject);
}

void CShader::AnimateObjects(float fTimeElapsed)
{
	for (auto& pObject : m_pObjects)
		pObject->Animate(fTimeElapsed);
}

void CShader::ReleaseObjcets()
{
	m_pObjects.clear();
}

void CShader::OnPrepareRender(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	pCommandList->SetPipelineState(m_ppPipelineStates[0]);
}

void CShader::Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	OnPrepareRender(pCommandList);

	for (auto& pObject : m_pObjects) {
		pObject->Render(pCommandList);
	}
}
