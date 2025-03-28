#include "Common\pch.h"
#include "ResourceManager.h"
#include "Mesh/Polygon/PolygonGenerator.h"

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
	m_shaderMap.clear();
}

CBaseMesh* CResourceManager::LoadMesh(const std::string& meshName)
{
	auto it = m_meshMap.find(meshName);
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
	CPolygonGenerator polyGen;
	auto CubePolygon = polyGen.GenFromObjModel("Cube");
	m_meshMap["Cube"] = std::make_unique<CBaseMesh>(pDevice, pCommandList, CubePolygon.get());

	auto TankPolygon = polyGen.GenFromObjModel("tank");
	m_meshMap["Tank"] = std::make_unique<CBaseMesh>(pDevice, pCommandList, TankPolygon.get());

	auto ConePolygon = polyGen.GenFromObjModel("Cone");
	m_meshMap["Cone"] = std::make_unique<CBaseMesh>(pDevice, pCommandList, ConePolygon.get());

	auto CylinderPolygon = polyGen.GenFromObjModel("Cylinder");
	m_meshMap["Cylinder"] = std::make_unique<CBaseMesh>(pDevice, pCommandList, CylinderPolygon.get());

	auto IcoSpherePolygon = polyGen.GenFromObjModel("IcoSphere");
	m_meshMap["IcoSphere"] = std::make_unique<CBaseMesh>(pDevice, pCommandList, IcoSpherePolygon.get());

	auto MonkeyPolygon = polyGen.GenFromObjModel("Monkey");
	m_meshMap["Monkey"] = std::make_unique<CBaseMesh>(pDevice, pCommandList, MonkeyPolygon.get());
}

void CResourceManager::BuildShader(ID3D12Device* pDevice)
{
	m_shaderMap["VSDefault"] = std::make_unique<CShader>(L"Assets/Shaders/Shaders.hlsl", "VSDefault", "vs_5_1");
	m_shaderMap["PSDefault"] = std::make_unique<CShader>(L"Assets/Shaders/Shaders.hlsl", "PSDefault", "ps_5_1");

	m_shaderMap["VSDiffused"] = std::make_unique<CShader>(L"Assets/Shaders/DiffusedShader.hlsl", "VSDiffused", "vs_5_1");
	m_shaderMap["PSDiffused"] = std::make_unique<CShader>(L"Assets/Shaders/DiffusedShader.hlsl", "PSDiffused", "ps_5_1");
}

void CResourceManager::BuildPSO(ID3D12Device* pDevice)
{
	auto pRootSig	= m_rootSignatureMap["Default"]->GetRootSignature();
	auto pDefaultVs = m_shaderMap["VSDefault"]->GetpShaderBlob();
	auto pDefaultPs = m_shaderMap["PSDefault"]->GetpShaderBlob();
	m_psoMap["Opaque"]		= std::make_unique<COpaquePSO>(pDevice, pRootSig, pDefaultVs, pDefaultPs);
	m_psoMap["OpaqueLine"]	= std::make_unique<COpaqueLinePSO>(pDevice, pRootSig, pDefaultVs, pDefaultPs);

	auto pDiffusedVs = m_shaderMap["VSDiffused"]->GetpShaderBlob();
	auto pDiffsuedPs = m_shaderMap["PSDiffused"]->GetpShaderBlob();
	m_psoMap["Diffused"]	= std::make_unique<CDiffusedPSO>(pDevice, pRootSig, pDiffusedVs, pDiffsuedPs);
}
