#pragma once
#include "Mesh.h"
#include "Renderer/Shader/Shader.h"
#include "Component/Component.h"
class CGameObject {
public:
	CGameObject();

	virtual void Update(float fElapsedTime);

protected:
	virtual void UpdateMeshes(float fElapsedTime);

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
