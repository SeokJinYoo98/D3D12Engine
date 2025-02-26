#pragma once

#include "GameTimer.h"

class CWindowManager {
public:
	CWindowManager() = default;

public:
	void Initialize(HINSTANCE hInst, HWND hWnd, int width=800, int height=600);
	void OnDestroy();
public:
	HINSTANCE	m_hInstance{ nullptr };
	HWND		m_hWnd{ nullptr };

	int m_clientWidth{ };
	int m_clientHeight{ };
};

class CD3D12DeviceManager {
public:
	CD3D12DeviceManager() = default;

public:
	void Initialize(const CWindowManager& windowManager);
	void OnDestroy();

	void ResetCommandListAndAllocator();
	void TransitionResourceFromPresentToRenderTarget(D3D12_RESOURCE_BARRIER& d3dResourceBarrier);
	void SetViewportAndScissorRect();
	void ClearRenderTargetAndDepthStencil(const DirectX::XMVECTOR& clearColor);
	void TransitionRenderTargetToPresent(D3D12_RESOURCE_BARRIER& d3dResourceBarrier);
	void ExcuteCommandList();
	void WaitForGpuComplete();
	void PresentSwapChain();

private:
	void InitializeDevice();
	void InitializeFence();
	void InitializeDescriptorSize();
	void Initialize4XMSAA();
	void InitializeCommandQueueAndList();
	void InitializeSwapChain(const CWindowManager& wdManager);
	void InitializeRtvAndDsvDescriptorHeap();
	void InitializeViewportAndScissorRect(const CWindowManager& wdManager);

	void CreateRenderTargetView();
	void CreateDepthStecilView(const CWindowManager& wdManager);
private:
	Microsoft::WRL::ComPtr<IDXGIFactory4>	m_dxgiFactory{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Device>	m_d3dDevice{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain3>	m_swapChain{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Fence>		m_d3dFence{ nullptr };
	UINT64 m_fenceValue{ 0 };
	HANDLE m_fenceEvent{ };

	UINT m_RtvDescriptorSize{ 0 };
	UINT m_DsvDescriptorSize{ 0 };
	UINT m_CbvSrcDescriptorSize{ 0 };

	UINT m_4xMsaaQuality{ 0 };
	BOOL m_4xMsaaEnable{ false };

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_commandQueue{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_directCmdListAlloc{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_rtvHeap{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_dsvHeap{ nullptr };
	UINT m_dsvDescriptorIncrementSize{ 0 };
	UINT m_rtvDescriptorIncrementSize{ 0 };

	static const UINT SwapChainBufferCount{ 2 };

	Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[SwapChainBufferCount]{ };
	UINT m_swapChainBufferIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer{ nullptr };

	D3D12_VIEWPORT  m_d3dViewport{ };
	D3D12_RECT		m_d3dScissorRect{ };
private:
	
};

class CGameFramework
{
public:
	CGameFramework();


public:
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);

	void FrameAdvance();
	void OnDestroy();

private:
	void ProcessInput();
	void AnimateObjects();

	void BuildObjects();

private:
	CWindowManager		m_windowManager;
	CD3D12DeviceManager	m_deviceManager;

	CGameTimer			m_gameTimer;
	_TCHAR				m_pszFrameRate[50];
};

