#pragma once

class CD3D12DeviceManager {
public:
	CD3D12DeviceManager() = default;

public:
	void Initialize(HWND hWnd, UINT nWidth, UINT nHeight);
	void OnDestroy();
	void OnResize();
	void FlushCommandQueue();

	void ResetCommandListAndAllocator();
	D3D12_RESOURCE_BARRIER TransitionResourceFromPresentToRenderTarget();
	void ClearRenderTargetAndDepthStencil(const DirectX::XMVECTOR& clearColor);
	void TransitionRenderTargetToPresent(D3D12_RESOURCE_BARRIER& d3dResourceBarrier);
	void ExcuteCommandList();
	void WaitForGpuComplete();
	void PresentSwapChain();

	void MoveToNextFrame();
private:
	void InitializeDevice();
	void InitializeFence();
	void InitializeDescriptorSize();
	void Initialize4XMSAA();
	void InitializeCommandQueueAndList();
	void InitializeSwapChain(HWND hWnd, UINT nWidth, UINT nHeight);
	void InitializeRtvAndDsvDescriptorHeap();

	void CreateRenderTargetView();
	void CreateDepthStecilView(HWND hWnd, UINT nWidth, UINT nHeight);

public:
	static const UINT nSwapChainBufferCount{ 2 };

	Microsoft::WRL::ComPtr<IDXGIFactory4>	m_pFactory{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Device>	m_pD3dDevice{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain3>	m_pSwapChain{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Fence>		m_pFence{ nullptr };
	UINT64 m_nFenceValues[nSwapChainBufferCount]{ };
	HANDLE m_hFenceEvent{ };

	UINT m_nRtvDescriptorSize{ 0 };
	UINT m_nDsvDescriptorSize{ 0 };
	UINT m_nCbvSrcDescriptorSize{ 0 };

	UINT m_nMsaa4xQuality{ 0 };
	BOOL m_bMsaa4xEnable{ false };

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_pCommandQueue{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pCommandAllocator{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pCommandList{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_pRtvHeap{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_pDsvHeap{ nullptr };
	UINT m_dsvDescriptorIncrementSize{ 0 };
	UINT m_rtvDescriptorIncrementSize{ 0 };



	Microsoft::WRL::ComPtr<ID3D12Resource> m_pSwapChainBuffers[nSwapChainBufferCount]{ };
	UINT m_nSwapChainBufferIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthStencilBuffer{ nullptr };
};