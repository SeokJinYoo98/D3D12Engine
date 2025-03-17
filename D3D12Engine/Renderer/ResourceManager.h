#pragma once
#include "Gfw/GameObject.h"
#include "Renderer/RootSignature/RootSignature.h"
class CResourceManager
{
public:
	CResourceManager() = default;
	void Init(ID3D12Device* pDevice);
	void OnDestroy();

	CRootSignature* LoadRootSig(const std::string& sigName);
private:
	void							DestroyRootSig();
	void							BuildRootSig(ID3D12Device* pDevice);
	void							MappingRootSig(const std::string& sigName, std::shared_ptr<CRootSignature> rootSig);
	std::shared_ptr<CRootSignature> GetRootSig(const std::string& sigName);

private:
	std::unordered_map<std::string, std::shared_ptr<CRootSignature>>	m_rootSignatureMap;
};

