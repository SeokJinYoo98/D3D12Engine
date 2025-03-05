#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_CAMERA_INFO
{
	DirectX::XMFLOAT4X4 m_xmf4x4View;
	DirectX::XMFLOAT4X4 m_xmf4x4Projection;
};

class CCamera
{
protected:
	DirectX::XMFLOAT4X4 m_xmf4x4View;
	DirectX::XMFLOAT4X4 m_xmf4x4Projection;

	D3D12_VIEWPORT	m_d3dViewport;
	D3D12_RECT		m_d3dScissorRect;

public:
	CCamera();
	virtual ~CCamera();

	virtual void CreateShaderVariables(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pCommandList);

	void OnResize(const RECT& rcClient);
	void GenerateViewMatrix(DirectX::XMFLOAT3 xmf3Position, DirectX::XMFLOAT3 xmf3LookAt, DirectX::XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pCommandList);
};

