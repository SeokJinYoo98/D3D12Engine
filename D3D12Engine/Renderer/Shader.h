#pragma once
#include "Gfw/Camera/Camera.h"

struct CB_GAMEOBJECT_INFO
{
	DirectX::XMFLOAT4X4 m_xmf4x4World;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();

public:
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppShaderBlob);

	virtual void CreateShader(ID3D12Device* pDevice, ID3D12RootSignature* pRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pCommandList, const DirectX::XMFLOAT4X4& xmf4x4World);
	virtual void ReleaseShaderVariables();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, CCamera* camera);

protected:
	// 파이프라인 상태 객체들의 
	ID3D12PipelineState** m_ppPipelineStates;
	int m_nPipelineStates{ 0 };
};

class CDiffusedShader :public CShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppShaderBlob);

	virtual void CreateShader(ID3D12Device* d3dDevice, ID3D12RootSignature* d3dGrapjicsRootSignature);
};
