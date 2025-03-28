#include "Common\pch.h"
#include "Gfw/Camera/Camera.h"

CCamera::CCamera()
	: m_xmf4x4View{ Matrix4x4::Identity() }, m_xmf4x4Projection{ Matrix4x4::Identity() },
	m_d3dViewport{0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f},
	m_d3dScissorRect{0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT}
{
}

CCamera::~CCamera()
{
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}


void CCamera::Update(float fElapsedTime)
{
	if (m_bDirty) {
		GenerateViewMatrix(m_xmf3Pos, m_xmf3LookAt, m_xmf3Up);
		m_bDirty = false;
	}
}

void CCamera::Render()
{
}

void CCamera::SetCameraPos(const DirectX::XMFLOAT3& xmf3Pos)
{
	m_xmf3Pos = xmf3Pos;
	if (!m_bDirty) m_bDirty = true;
}

void CCamera::SetCameraLookAt(const DirectX::XMFLOAT3& xmf3LookAt)
{
	m_xmf3LookAt = xmf3LookAt;
	if (!m_bDirty) m_bDirty = true;
}

void CCamera::SetCameraUp(const DirectX::XMFLOAT3& xmf3Up)
{
	m_xmf3Up = xmf3Up;
	if (!m_bDirty) m_bDirty = true;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(DirectX::XMConvertToRadians(fFovAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::OnResize(const RECT& rcClient)
{
}

void CCamera::GenerateViewMatrix(const DirectX::XMFLOAT3& xmf3Pos, const DirectX::XMFLOAT3& xmf3LookAt, const DirectX::XMFLOAT3& xmf3Up)
{
	m_xmf4x4View = Matrix4x4::LookAtLH(xmf3Pos, xmf3LookAt, xmf3Up);
}
