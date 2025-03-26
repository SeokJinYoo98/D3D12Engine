#pragma once
#include "Renderer/RootSignature/RootSignature.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/PSO/PSO.h"
#include "Renderer/Mesh/Mesh.h"

class CResourceManager
{
public:
	CResourceManager() { }
	void InitBegin(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void InitEnd();

	CBaseMesh*						LoadNewMesh(const std::string& meshName);
	ID3D12RootSignature*			LoadRootSig(const std::string& sigName);
	ID3D12PipelineState*			LoadPSO(const std::string& psoName);

private:
	void					BuildRootSig(ID3D12Device* pDevice);
	void					BuildMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void					BuildShader(ID3D12Device* pDevice);
	void					BuildPSO(ID3D12Device* pDevice);

private:

	std::unordered_map<std::string, std::unique_ptr<CRootSignature>>	m_rootSignatureMap;

	std::unordered_map<std::string, std::unique_ptr<CShader>>			m_shaderMap;
	std::unordered_map<std::string, std::unique_ptr<CPSO>>				m_psoMap;
	std::unordered_map<std::string, std::unique_ptr<CBaseMesh>>			m_newMeshMap;
};

