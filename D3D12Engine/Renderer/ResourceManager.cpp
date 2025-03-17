#include "Common\pch.h"
#include "ResourceManager.h"

void CResourceManager::Init(ID3D12Device* pDevice)
{
	BuildRootSig(pDevice);
}

void CResourceManager::OnDestroy()
{
	DestroyRootSig();
}

CRootSignature* CResourceManager::LoadRootSig(const std::string& sigName)
{
	return GetRootSig(sigName).get();
}

void CResourceManager::DestroyRootSig()
{
	for (auto& [name, sig] : m_rootSignatureMap)
		sig->Destroy();
}

void CResourceManager::BuildRootSig(ID3D12Device* pDevice)
{
	auto defaultRoot = std::make_shared<CRootSignature>();
	MappingRootSig("Default", defaultRoot);

	for (auto& [name, sig] : m_rootSignatureMap)
		sig->Init(pDevice);
}

void CResourceManager::MappingRootSig(const std::string& sigName, std::shared_ptr<CRootSignature> rootSig)
{
	auto it = m_rootSignatureMap.find(sigName);
	if (it == m_rootSignatureMap.end())
		m_rootSignatureMap[sigName] = rootSig;
}

std::shared_ptr<CRootSignature> CResourceManager::GetRootSig(const std::string& sigName)
{
	auto it = m_rootSignatureMap.find(sigName);
	if (it == m_rootSignatureMap.end())
		return nullptr;
	return it->second;
}
