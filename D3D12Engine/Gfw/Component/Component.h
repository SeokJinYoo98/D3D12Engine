#pragma once
class CComponent
{
public: 
	CComponent() = default;
};

class CTransformComponent : public CComponent{
};
class CMovementComponent : public CComponent {
	friend class CTransformComponent;
};
class CMeshGeometry;
class CMatrial;

class CStaticMeshComponent {
public:
	CStaticMeshComponent();
	DirectX::XMFLOAT4X4			GetTransform() const;
	const DirectX::XMFLOAT4&	GetColor() const;

	void Rotate(float x, float y, float z, float fAngle);
	void Move(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Rotate(const DirectX::XMFLOAT3& xmf3Axis, float fAngle);
	void Move(const DirectX::XMFLOAT3& xmf3Pos);
	void Scale(const DirectX::XMFLOAT3& xmf3Scale);

	int m_nFramesDirty						= 3;
	std::string				m_strMeshName	= { "None" };
	DirectX::XMFLOAT4X4*	m_pWorld		= nullptr;
	DirectX::XMFLOAT4X4		m_xmf4x4Local;
	DirectX::XMFLOAT4		m_xmf4Color		= { 0.f, 0.f, 0.f, 1.f };
};
