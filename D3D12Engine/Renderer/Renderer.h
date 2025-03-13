#pragma once
#include "Renderer/D3D12Device/D3D12Device.h"

class CRenderer : public CD3D12Device
{
public:
	CRenderer();

	void InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight);

	void BeginRendering(const DirectX::XMVECTOR& xmvClearColor);
	void EndRendering();

	ID3D12GraphicsCommandList	*GetCommandList()	const { return m_pCommandList.Get(); }
	ID3D12Device				*GetDevice()		const { return m_pD3dDevice.Get(); }
private:

};

