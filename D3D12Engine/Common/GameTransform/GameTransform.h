#pragma once
namespace TransformHelper {

}
class CGameTransform 
{
public:
	CGameTransform();
	void SetParent(std::shared_ptr<CGameTransform> pParent) {
		if (!pParent)
			throw std::runtime_error("Invalid parent");
		m_pParentTransform = pParent;
	}
	DirectX::XMFLOAT4X4 GetTransform();
	DirectX::XMFLOAT3	GetPosition();
	DirectX::XMFLOAT3	GetForwardVector();
	DirectX::XMFLOAT3	GetBackwardVector();
	DirectX::XMFLOAT3   GetRightVector();
	DirectX::XMFLOAT3   GetLeftVector();
	DirectX::XMFLOAT3	GetPosPlusForward();

	void SetScaleX(float fScaleX);
	void SetScaleY(float fScaleY);
	void SetScaleZ(float fScaleZ);
	void SetScale(float fScaleX, float fScaleY, float fScaleZ);
	void SetScale(const DirectX::XMFLOAT3& xmf3Scale);

	void SetPositionX(float fPosX);
	void SetPositionY(float fPosY);
	void SetPositionZ(float fPosZ);
	void SetPosition(float fPosX, float fPosY, float fPosZ);
	void SetPosition(const DirectX::XMFLOAT3& xmf3Position);

	void SetRotationYaw(float fYaw);
	void SetRotationPitch(float fPitch);
	void SetRotationRoll(float fRoll);
	void SetRotation(float fYaw, float fPitch, float fRoll);
	void SetRotation(const DirectX::XMFLOAT3& xmf3Roatation);

	void Translate(float x, float y, float z);
	void Translate(const DirectX::XMFLOAT3& xmf3DeltaPosition);
	void Rotate(float x, float y, float z);
	void Rotate(const DirectX::XMFLOAT3& xmf3DeltaRotate);
	void Scale(float x, float y, float z);
	void Scale(const DirectX::XMFLOAT3& xmf3DeltaScale);

private:
	void UpdateTransform();

private:
	std::shared_ptr<CGameTransform>		m_pParentTransform = nullptr;
	DirectX::XMFLOAT4X4					m_xmf4x4Transform;

	DirectX::XMFLOAT3					m_xmf3Position;
	DirectX::XMFLOAT3					m_xmf3Rotation;
	DirectX::XMFLOAT3					m_xmf3Scale;

	bool m_bDirtyTransform = false;
};

class CCameraTransform {
public:
	CCameraTransform() = default;
};