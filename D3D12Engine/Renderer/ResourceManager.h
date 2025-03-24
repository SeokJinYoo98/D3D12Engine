#pragma once
#include "Renderer/RootSignature/RootSignature.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/PSO/PSO.h"
#include "Gfw/Mesh.h"

class CResourceManager
{
public:
	static CResourceManager& GetInstance() {
		static CResourceManager instance;
		return instance;
	}
	CResourceManager() { }
	void InitBegin(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void InitEnd();

	std::shared_ptr<CMesh>	LoadSharedMesh(const std::string& meshName);
	CMesh*					LoadMesh(const std::string& name);
	ID3D12RootSignature*	LoadRootSig(const std::string& sigName);
	ID3D12PipelineState*	LoadPSO(const std::string& psoName);

private:
	void					BuildRootSig(ID3D12Device* pDevice);
	void					BuildMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void					BuildShader(ID3D12Device* pDevice);
	void					BuildPSO(ID3D12Device* pDevice);

private:
	std::unordered_map<std::string, std::shared_ptr<CMesh>>				m_meshSharedMap;
	std::unordered_map<std::string, std::unique_ptr<CRootSignature>>	m_rootSignatureMap;
	std::unordered_map<std::string, std::unique_ptr<CMesh>>				m_meshMap;
	std::unordered_map<std::string, std::unique_ptr<CShader>>			m_shaderMap;
	std::unordered_map<std::string, std::unique_ptr<CPSO>>				m_psoMap;
};

