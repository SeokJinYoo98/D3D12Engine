#pragma once
#include "GameObject.h"

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

	virtual void CreateShader(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& pRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList, void* pConstext = nullptr);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjcets();

	virtual void OnPrepareRender(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);
	virtual void Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);

protected:
	// 셰이더가 포함하는 게임 객체들의 리스트
	std::vector<std::shared_ptr<CGameObject>> m_pObjects;

	// 파이프라인 상태 객체들의 리스트
	ID3D12PipelineState** m_ppPipelineStates{ nullptr };
	int m_nPipelineStates{ };
};

