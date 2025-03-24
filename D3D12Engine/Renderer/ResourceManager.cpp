#include "Common\pch.h"
#include "ResourceManager.h"

void CResourceManager::InitBegin(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	BuildRootSig(pDevice);
	BuildShader(pDevice);
	BuildPSO(pDevice);
	BuildMesh(pDevice, pCommandList);
}

void CResourceManager::InitEnd()
{
	for (auto& [name, mesh] : m_meshMap)
		mesh->ReleaseUploadBuffers();
	for (auto& [name, mesh] : m_meshSharedMap)
		mesh->ReleaseUploadBuffers();
}

std::shared_ptr<CMesh> CResourceManager::LoadSharedMesh(const std::string& meshName)
{
	auto it = m_meshSharedMap.find(meshName);
	if (it == m_meshSharedMap.end())
		return nullptr;
	return it->second;
}

CMesh* CResourceManager::LoadMesh(const std::string& name)
{
	auto it = m_meshMap.find(name);
	if (it == m_meshMap.end())
		return nullptr;
	return it->second.get();
}

ID3D12RootSignature* CResourceManager::LoadRootSig(const std::string& sigName)
{
	auto it = m_rootSignatureMap.find(sigName);
	if (it == m_rootSignatureMap.end())
		return nullptr;
	return it->second->GetRootSignature();
}

ID3D12PipelineState* CResourceManager::LoadPSO(const std::string& psoName)
{
	auto it = m_psoMap.find(psoName);
	if (it == m_psoMap.end()) 
		return nullptr;
	
	return it->second->GetPSO();
}

void CResourceManager::BuildRootSig(ID3D12Device* pDevice)
{
	m_rootSignatureMap["Default"] = std::make_unique<CRootSignature>(pDevice);
}

void CResourceManager::BuildMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_meshMap["Cube"] = std::make_unique<CCubeMesh>(pDevice, pCommandList);
	m_meshSharedMap["Cube"] = std::make_shared<CCubeMesh>(pDevice, pCommandList);
}

void CResourceManager::BuildShader(ID3D12Device* pDevice)
{
	m_shaderMap["Vs"] = std::make_unique<CShader>(L"Assets/Shaders/Shaders.hlsl", "VSDiffused", "vs_5_1");
	m_shaderMap["Ps"] = std::make_unique<CShader>(L"Assets/Shaders/Shaders.hlsl", "PSDiffused", "ps_5_1");
}

void CResourceManager::BuildPSO(ID3D12Device* pDevice)
{
	auto pRootSig = m_rootSignatureMap["Default"]->GetRootSignature();
	auto pVs = m_shaderMap["Vs"]->GetpShaderBlob();
	auto pPs = m_shaderMap["Ps"]->GetpShaderBlob();

	m_psoMap["Opaque"]		= std::make_unique<COpaquePSO>(pDevice, pRootSig, pVs, pPs);
	m_psoMap["OpaqueLine"]	= std::make_unique<COpaqueLinePSO>(pDevice, pRootSig, pVs, pPs);
}
