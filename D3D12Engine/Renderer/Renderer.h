#pragma once
#include "Renderer/D3D12Device/D3D12Device.h"

class CResourceManager;
class CRenderer : public CD3D12Device
{
public:
	CRenderer();

	void InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight);

	void BeginRendering(const DirectX::XMVECTOR& xmvClearColor);
	void EndRendering();

	ID3D12DescriptorHeap** GetCbvHeap() { return m_pCbvHeap.GetAddressOf(); }
private:
	void	BuildCbvDescriptorHeaps();
	void	BuildResourceManager();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_pCbvHeap = nullptr;
	UINT m_nCbvDescriptorSize = { };

	CResourceManager*	m_pResourceManager;
};

