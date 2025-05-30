#pragma once

class CD3D12Device
{
public:
	CD3D12Device();

	ID3D12GraphicsCommandList*	GetCommandList()	const { return m_pCommandList.Get(); }
	ID3D12Device*				GetDevice()		const { return m_pDevice.Get(); }

	virtual void InitDevice(HWND hWnd, UINT nWidth, UINT nHeight);
	virtual void OnResize(HWND hWnd, UINT nWidth, UINT nHeight);
	virtual void OnDestroy();

	void WaitForGpuComplete();
	void ExcuteCommandList();
	void ResetCommandList();

private:
	void CreateCommandObject();
	void CreateSwapChain(HWND hWnd, UINT nWidth, UINT nHeight);
	void CreateRtvAndDsvDescriptorHeap();

	void CreateRenderTargetView();
	void CreateDepthStecilView(HWND hWnd, UINT nWidth, UINT nHeight);

protected:
	void ResetCommandAlloc();
	void ResetCommandAlloc(ID3D12CommandAllocator* pCmdAlloc, ID3D12PipelineState* pPso = nullptr);
	void TransitionResourceFromPresentToRenderTarget();
	void ClearRenderTargetAndDepthStencil(const DirectX::XMFLOAT4& clearColor);
	void TransitionRenderTargetToPresent();
	void PresentSwapChain();
	void MoveToNextFrame();

	virtual void CreateCbvDescriptorHeap(UINT nMesh, UINT nUI, UINT nPass);
protected:
	static constexpr UINT SWAP_CHAIN_FRAME_COUNT = 2;

	Microsoft::WRL::ComPtr<IDXGIFactory4>	m_pFactory		= nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device>	m_pDevice		= nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain3>	m_pSwapChain	= nullptr;

	Microsoft::WRL::ComPtr<ID3D12Fence>			m_pFence = nullptr;
	std::array<UINT64, SWAP_CHAIN_FRAME_COUNT>	m_nFenceValues = { };
	HANDLE m_hFenceEvent = nullptr;

	UINT m_nMsaa4xQuality	= { };
	BOOL m_bMsaa4xEnable	= false;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_pCommandQueue		= nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pCommandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pCommandList		= nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_pRtvHeap	= nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_pDsvHeap	= nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_pCbvHeap	= nullptr;
	UINT m_nRtvDescriptorSize = { };
	UINT m_nDsvDescriptorSize = { };
	UINT m_nCbvDescriptorSize = { };

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pSwapChainBuffers[SWAP_CHAIN_FRAME_COUNT]{ };
	UINT m_nSwapChainBufferIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthStencilBuffer{ nullptr };

protected:
	DXGI_FORMAT	m_d3dBackBufferFormat;
};

