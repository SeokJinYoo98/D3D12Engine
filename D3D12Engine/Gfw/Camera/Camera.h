#pragma once
#include "Renderer/Shader/Shader.h"
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_CAMERA_INFO
{
	DirectX::XMFLOAT4X4 m_xmf4x4View;
	DirectX::XMFLOAT4X4 m_xmf4x4Projection;
};

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	virtual void Update(float fElapsedTime);
	virtual void Render();

	void SetCameraPos(const DirectX::XMFLOAT3& xmf3Pos);
	void SetCameraLookAt(const DirectX::XMFLOAT3& xmf3LookAt);
	void SetCameraUp(const DirectX::XMFLOAT3& xmf3Up);

	void OnResize(const RECT& rcClient);
	void GenerateViewMatrix(const DirectX::XMFLOAT3& xmf3Pos, const DirectX::XMFLOAT3& xmf3LookAt, const DirectX::XMFLOAT3& xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	const D3D12_RECT*		GetScissorRect() const { return &m_d3dScissorRect; }
	const D3D12_VIEWPORT*	GetViewport()	 const { return &m_d3dViewport; }
	const DirectX::XMFLOAT4X4& GetView()	 const { return m_xmf4x4View; }
	const DirectX::XMFLOAT4X4& GetProj()	 const { return m_xmf4x4Projection; }
public:

protected:
	DirectX::XMFLOAT4X4 m_xmf4x4View = { };
	DirectX::XMFLOAT4X4 m_xmf4x4Projection = { };

	D3D12_VIEWPORT	m_d3dViewport = { };
	D3D12_RECT		m_d3dScissorRect = { };

	DirectX::XMFLOAT3 m_xmf3Pos = { };
	DirectX::XMFLOAT3 m_xmf3LookAt = { };
	DirectX::XMFLOAT3 m_xmf3Up = { };

	float m_fNearPlane = 1.f;
	float m_fFarPlane = 100.f;
	float m_fRatio = 0.f;
	float m_fFov = 45.f;

	bool m_bDirty = false;
};

