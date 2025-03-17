#include "Common\pch.h"
#include "Renderer/Shader/Shader.h"

CShader::CShader()
{
}

CShader::~CShader()
{
}

// 래스터라이저 상태를 설정하기 위한 구조체를 반환
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc{ };
	d3dRasterizerDesc.FillMode				= D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode				= D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias				= 0;
	d3dRasterizerDesc.DepthBiasClamp		= 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias	= 0.0f;
	d3dRasterizerDesc.DepthClipEnable		= TRUE;
	d3dRasterizerDesc.MultisampleEnable		= FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount		= 0;
	d3dRasterizerDesc.ConservativeRaster	= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3dRasterizerDesc;
}

// Depth-Stencil 검사를 위한 상태를 설정하기 위한 구조체
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc{ };
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

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc{ };
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

// 입력 조립기에서 정점 버퍼의 구조를 알려주기 위한 구조체를 반환.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{ };
	d3dInputLayoutDesc.pInputElementDescs = nullptr;
	d3dInputLayoutDesc.NumElements = 0;

	return d3dInputLayoutDesc;
}

// 정점 셰이더 바이트 코드를 생성한다.
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode{ };
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = nullptr;
	return d3dShaderByteCode;
}
// 픽셀 셰이더 바이트 코드를 생성한다.
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode{ };
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = nullptr;
	return d3dShaderByteCode;
}
// 셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다.
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppShaderBlob)
{
	UINT nComPileFlags{ };
#if defined(_DEBUG)
	nComPileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

	HRESULT hResult = ::D3DCompileFromFile(
		pszFileName, nullptr, nullptr,
		pszShaderName, pszShaderProfile,
		nComPileFlags, 0, ppShaderBlob, pErrorBlob.GetAddressOf()
	);

	if (FAILED(hResult))
	{
		if (pErrorBlob)
		{ 
			::OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			std::cout << (char*)pErrorBlob->GetBufferPointer() << std::endl;
		}
		// 에러 처리 또는 함수 종료
		throw std::runtime_error("Shader compilation failed.");
	}

	D3D12_SHADER_BYTECODE d3dShaderByteCode{ };
	d3dShaderByteCode.BytecodeLength = (*ppShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

// 그래픽스 파이프라인 상태 객체를 생성한다.
void CShader::CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature, const std::string& psoName)
{
	m_pPSOs[psoName] = nullptr;
	ID3DBlob* pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc{ };
	d3dPipelineStateDesc.pRootSignature			= pRootSignature;
	d3dPipelineStateDesc.VS						= CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS						= CreatePixelShader(&pd3dPixelShaderBlob);
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
		IID_PPV_ARGS(m_pPSOs[psoName].GetAddressOf())
	);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

void CShader::BuildCbvDescriptorHeaps(ID3D12Device* pDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	::ZeroMemory(&cbvHeapDesc, sizeof(cbvHeapDesc));
	cbvHeapDesc.NumDescriptors	= 3 + 1;
	cbvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask		= 0;
	HRESULT hResult = pDevice->CreateDescriptorHeap(
		&cbvHeapDesc,
		IID_PPV_ARGS(m_pCbvHeap.GetAddressOf())
	);

	if (FAILED(hResult))
		throw std::runtime_error("Failed to Create Cbv Heap");

	m_nCbvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CShader::BuildConstantBuffers(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12DescriptorHeap** pCbvHeap, UINT nElementCount)
{
	m_pObjectCBs = std::make_unique<CUploadBuffer<CB_Object>>(pDevice, nElementCount, true);
	m_pPassCBs = std::make_unique<CUploadBuffer<CB_Pass>>(pDevice, 1, true);

	UINT objCBByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(CB_Object));
	UINT objCount = nElementCount;
	auto objectCB = m_pObjectCBs->Resource();
	for (int i = 0; i < objCount; ++i) {
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();
		cbAddress += (i * objCBByteSize);

		int heapIndex = /*FrameIndex x ObjCount x */ i;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, m_nCbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = objCBByteSize;
		
		pDevice->CreateConstantBufferView(&cbvDesc, handle);
	}

	UINT passCBByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(CB_Pass));
	auto passCB = m_pPassCBs->Resource();
	for (int i = objCount; i < objCount + 1; ++i) {
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		int heapIndex = i;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, m_nCbvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		pDevice->CreateConstantBufferView(&cbvDesc, handle);
	}
	passIndex = 2;
}

void CShader::UpdateObjectConstant(CB_Object cbData, UINT cbvIndex)
{
	currIndex = cbvIndex;
	DirectX::XMStoreFloat4x4(&cbData.m_xmf4x4World, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&cbData.m_xmf4x4World)));
	m_pObjectCBs->CopyData(currIndex, cbData);
}

void CShader::UpdatePassConstant(CB_Pass cbPass)
{
	XMStoreFloat4x4(&cbPass.m_xmf4x4View, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&cbPass.m_xmf4x4View)));
	XMStoreFloat4x4(&cbPass.m_xmf4x4Projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&cbPass.m_xmf4x4Projection)));

	m_pPassCBs->CopyData(0, cbPass);
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList)
{
}


void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pCommandList, const DirectX::XMFLOAT4X4& xmf4x4World)
{
	CB_Object CBGameObjectInfo;
	XMStoreFloat4x4(&CBGameObjectInfo.m_xmf4x4World, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&xmf4x4World)));
	pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(CB_Object) / 4, &CBGameObjectInfo.m_xmf4x4World, 0);
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::UpdatePassCB(ID3D12GraphicsCommandList* pCommandList)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pCbvHeap.Get() };
	pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	gpuHandle.Offset(passIndex, m_nCbvDescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(1, gpuHandle);
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pCommandList, const std::string& psoName, UINT cbIndex)
{
	currIndex = cbIndex;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
	gpuHandle.Offset(currIndex, m_nCbvDescriptorSize);
	std::cout << currIndex << ", " << gpuHandle.ptr << std::endl;

	pCommandList->SetGraphicsRootDescriptorTable(0, gpuHandle);
	pCommandList->SetPipelineState(m_pPSOs[psoName].Get());
}

void CShader::Render(ID3D12GraphicsCommandList* pCommandList, const std::string& psoName, UINT cbIndex)
{
	OnPrepareRender(pCommandList, psoName, cbIndex);
}

CDiffusedShader::CDiffusedShader()
{
}

CDiffusedShader::~CDiffusedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CDiffusedShader::CreateInputLayout()
{
/*
	D3D12_INPUT_ELEMENT_DESC
	- LPCSTR						SemanticName;
	- UINT							SemanticIndex;
	- DXGI_FORMAT					Format;
	- UINT							InputSlot;
	- UINT							AlignedByteOffset;
	- D3D12_INPUT_CLASSIFICATION	InputSlotClass;
	- UINT							InstanceDataStepRate;
*/
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
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			12,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{ };
	d3dInputLayoutDesc.pInputElementDescs = m_vInputLayouts.data();
	d3dInputLayoutDesc.NumElements = static_cast<UINT>(m_vInputLayouts.size());

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreateVertexShader(ID3DBlob** ppShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Assets/Shaders/Shaders.hlsl", "VSDiffused", "vs_5_1", ppShaderBlob);
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreatePixelShader(ID3DBlob** ppShaderBlob)
{
	return CShader::CompileShaderFromFile(L"Assets/Shaders/Shaders.hlsl", "PSDiffused", "ps_5_1", ppShaderBlob);
}

void CDiffusedShader::CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pGraphicsRootSignature, const std::string& psoName)
{
	if (m_pPSOs[psoName])
		return;

	CShader::CreateShader(pDevice, pGraphicsRootSignature, psoName);
}
