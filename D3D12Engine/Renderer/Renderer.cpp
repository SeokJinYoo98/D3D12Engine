#include "Common\pch.h"
#include "Renderer.h"

CRenderer::CRenderer()
{
}

void CRenderer::InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight)
{
	CD3D12Device::InitDevice(hWnd, nWidth, nHeight);
}

void CRenderer::BeginRendering(const DirectX::XMVECTOR& xmvClearColor)
{
	ResetCommandAlloc();
	TransitionResourceFromPresentToRenderTarget();
	ClearRenderTargetAndDepthStencil(xmvClearColor);
}

void CRenderer::EndRendering()
{
	TransitionRenderTargetToPresent();
	ExcuteCommandList();
	WaitForGpuComplete();
	PresentSwapChain();
	MoveToNextFrame();
}
