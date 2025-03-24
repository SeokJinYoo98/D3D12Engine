#include "Common\pch.h"
#include "PSO.h"

void CPSO::CreatePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* pVS, ID3DBlob* pPS)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc{ };
	d3dPipelineStateDesc.pRootSignature			= pRootSig;
	d3dPipelineStateDesc.VS						= CreateShaderByteCode(pVS);
	d3dPipelineStateDesc.PS						= CreateShaderByteCode(pPS);
	d3dPipelineStateDesc.RasterizerState		= CreateRasterizerState();
	d3dPipelineStateDesc.BlendState				= CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState		= CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout			= CreateInputLayout();
	d3dPipelineStateDesc.SampleMask				= UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets		= 1;
	d3dPipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count		= 1;
	d3dPipelineStateDesc.Flags					= D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pDevice->CreateGraphicsPipelineState(
		&d3dPipelineStateDesc,
		IID_PPV_ARGS(m_pPSO.GetAddressOf())
	);
}

D3D12_RASTERIZER_DESC CPSO::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode					= D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode					= D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise		= FALSE;
	d3dRasterizerDesc.DepthBias					= 0;
	d3dRasterizerDesc.DepthBiasClamp			= 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias		= 0.0f;
	d3dRasterizerDesc.DepthClipEnable			= TRUE;
	d3dRasterizerDesc.MultisampleEnable			= FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable		= FALSE;
	d3dRasterizerDesc.ForcedSampleCount			= 0;
	d3dRasterizerDesc.ConservativeRaster		= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3dRasterizerDesc;
}

D3D12_DEPTH_STENCIL_DESC CPSO::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	d3dDepthStencilDesc.DepthEnable						= TRUE;
	d3dDepthStencilDesc.DepthWriteMask					= D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc						= D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable					= FALSE;
	d3dDepthStencilDesc.StencilReadMask					= 0x00;
	d3dDepthStencilDesc.StencilWriteMask				= 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp	= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc			= D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp		= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc			= D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

D3D12_BLEND_DESC CPSO::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc{ };
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));

	d3dBlendDesc.AlphaToCoverageEnable					= FALSE;
	d3dBlendDesc.IndependentBlendEnable					= FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable			= FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable			= FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend				= D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend				= D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp				= D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha			= D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha			= D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha			= D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp				= D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_INPUT_LAYOUT_DESC CPSO::CreateInputLayout()
{
	m_vInputLayouts =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	::ZeroMemory(&d3dInputLayoutDesc, sizeof(D3D12_INPUT_LAYOUT_DESC));
	d3dInputLayoutDesc.pInputElementDescs = m_vInputLayouts.data();
	d3dInputLayoutDesc.NumElements = static_cast<UINT>(m_vInputLayouts.size());

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CPSO::CreateShaderByteCode(ID3DBlob* pBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	::ZeroMemory(&d3dShaderByteCode, sizeof(D3D12_SHADER_BYTECODE));

	d3dShaderByteCode.BytecodeLength = pBlob->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = pBlob->GetBufferPointer();

	return d3dShaderByteCode;
}

COpaquePSO::COpaquePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* pVS, ID3DBlob* pPS)
{
	CPSO::CreatePSO(pDevice, pRootSig, pVS, pPS);
}

COpaqueLinePSO::COpaqueLinePSO(ID3D12Device* pDevice, ID3D12RootSignature* pRootSig, ID3DBlob* pVS, ID3DBlob* pPS)
{
	CPSO::CreatePSO(pDevice, pRootSig, pVS, pPS);
}

D3D12_RASTERIZER_DESC COpaqueLinePSO::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC myRaster = CPSO::CreateRasterizerState();
	myRaster.FillMode = D3D12_FILL_MODE_WIREFRAME;
	return myRaster;
}
