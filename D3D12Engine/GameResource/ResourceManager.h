#pragma once
#include "GameResource/RootSignature/RootSignature.h"
#include "GameResource/Shader/Shader.h"
#include "GameResource/PSO/PSO.h"
#include "GameResource/Mesh/Mesh.h"
#include "GameResource/Material/Material.h"
#include "GameResource/Texture/Texture.h"

class CResourceManager
{
public:
	CResourceManager() { }
	void InitBegin(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void InitEnd();

	CBaseMesh*						LoadMesh(const std::string& meshName);
	ID3D12RootSignature*			LoadRootSig(const std::string& sigName);
	ID3D12PipelineState*			LoadPSO(const std::string& psoName);
	std::shared_ptr<CBaseMesh>		LoadSharedMesh(const std::string& meshName);
	std::shared_ptr<CMaterial>		LoadMat(const std::string& matName);

	std::unordered_map<std::string, std::shared_ptr<CMaterial>>			m_materialMap;
private:
	void							BuildRootSig(ID3D12Device* pDevice);
	void							BuildMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void							BuildShader(ID3D12Device* pDevice);
	void							BuildPSO(ID3D12Device* pDevice);
	void							BuildMaterial();
	void							BuildTexture(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);

private:

	std::unordered_map<std::string, std::unique_ptr<CRootSignature>>	m_rootSignatureMap;
	std::unordered_map<std::string, std::unique_ptr<CShader>>			m_shaderMap;
	std::unordered_map<std::string, std::unique_ptr<CPSO>>				m_psoMap;
	std::unordered_map<std::string, std::shared_ptr<CBaseMesh>>			m_meshMap;
	std::unordered_map<std::string, std::shared_ptr<CTexture>>			m_textureMap;
	
};

