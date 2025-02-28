#pragma once
#include "Mesh.h"

class CShader;

class CGameObject {
public:
	CGameObject();
	virtual ~CGameObject();

protected:
	XMFLOAT4X4 m_xmf4x4World;

	std::shared_ptr<CMesh> m_pMesh;
	std::shared_ptr<CShader> m_pShader;

public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(std::shared_ptr<CMesh> pMesh);
	virtual void SetShader(std::shared_ptr<CShader> pShader);

	virtual void Animate(float fTimeElpased);

	virtual void OnPrepareRender();
	virtual void Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);
};

