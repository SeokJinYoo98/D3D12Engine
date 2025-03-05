#include "Common\pch.h"
#include "Gfw\Camera.h"

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

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->RSSetViewports(1, &m_d3dViewport);
	pCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(DirectX::XMConvertToRadians(fFovAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::OnResize(const RECT& rcClient)
{
}

void CCamera::GenerateViewMatrix(DirectX::XMFLOAT3 xmf3Position, DirectX::XMFLOAT3 xmf3LookAt, DirectX::XMFLOAT3 xmf3Up)
{
	m_xmf4x4View = Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);
}


void CCamera::CreateShaderVariables(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
}

void CCamera::ReleaseShaderVariables()
{
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList)
{
	VS_CB_CAMERA_INFO CBCameraInfo;

	XMStoreFloat4x4(&CBCameraInfo.m_xmf4x4View, DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&CBCameraInfo.m_xmf4x4Projection, DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));

	pCommandList->SetGraphicsRoot32BitConstants(1, sizeof(VS_CB_CAMERA_INFO) / 4, &CBCameraInfo, 0);
}
