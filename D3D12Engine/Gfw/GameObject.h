#pragma once
#include "Mesh.h"
#include "Renderer/Shader/Shader.h"

class CGameObject 
{
public:
	CGameObject();
	virtual ~CGameObject();
	UINT CbIndex{ };
protected:
	DirectX::XMFLOAT4X4 m_xmf4x4World;

	std::shared_ptr<CMesh> m_pMesh;
	std::shared_ptr<CShader> m_pShader;
	
public:
	void Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle);
	void Move(const DirectX::XMFLOAT3& xmf3Pos);
public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(std::shared_ptr<CMesh> pMesh);
	virtual void SetShader(std::shared_ptr<CShader> pShader);

	virtual void Update(float fTimeElpased);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pCommandList, CCamera* pCamera);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

private:
	DirectX::XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	inline void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	inline void SetRotationAxis(const DirectX::XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Update(float fTimeElapsed);
};