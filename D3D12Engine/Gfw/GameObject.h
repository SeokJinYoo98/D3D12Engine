#pragma once
#include "Mesh.h"
#include "Renderer/Shader/Shader.h"
#include "Component/Component.h"
class CNewObject {
public:
	CNewObject();

public:
	void Rotate(float x, float y, float z, float fAngle);
	void Move(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle);
	void Move(const DirectX::XMFLOAT3& xmf3Pos);
	void Scale(const DirectX::XMFLOAT3& xmf3Scale);

public:
	void AddMeshComponent(std::shared_ptr<CStaticMeshComponent> pMesh);
	const DirectX::XMFLOAT4X4& GetWorld() { return m_xmf4x4World; }

protected:
	DirectX::XMFLOAT4X4 m_xmf4x4World;
	std::vector<std::shared_ptr<CStaticMeshComponent>> m_pMeshes;
};
class CGameObject 
{
public:
	CGameObject();
	virtual ~CGameObject();
	UINT CbIndex{ };
	
	DirectX::XMFLOAT4 m_xmf4Color{ };
protected:
	DirectX::XMFLOAT4X4 m_xmf4x4World;

	std::shared_ptr<CMesh> m_pMesh;
	std::shared_ptr<CGameShader> m_pShader;
	
public:
	void Rotate(float x, float y, float z, float fAngle);
	void Move(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle);
	void Move(const DirectX::XMFLOAT3& xmf3Pos);
	void Scale(const DirectX::XMFLOAT3& xmf3Scale);
public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(std::shared_ptr<CMesh> pMesh);
	virtual void SetShader(std::shared_ptr<CGameShader> pShader);

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