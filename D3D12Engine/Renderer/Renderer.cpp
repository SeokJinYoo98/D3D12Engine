#include "Common\pch.h"
#include "Renderer.h"

CRenderer::CRenderer()
{
}

void CRenderer::InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight)
{
	CD3D12Device::InitDevice(hWnd, nWidth, nHeight);
	//BuildCbvDescriptorHeaps();
}

void CRenderer::BeginRendering(const DirectX::XMVECTOR& xmvClearColor)
{
	ResetCommandAlloc();
	TransitionResourceFromPresentToRenderTarget();
	ClearRenderTargetAndDepthStencil(xmvClearColor);

	//ID3D12DescriptorHeap* descriptorHeaps[] = { m_pCbvHeap.Get() };
	//m_pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//m_pCommandList->SetGraphicsRootDescriptorTable(1, m_pCbvHeap->GetGPUDescriptorHandleForHeapStart());
}

void CRenderer::EndRendering()
{
	TransitionRenderTargetToPresent();
	ExcuteCommandList();
	WaitForGpuComplete();
	PresentSwapChain();
	MoveToNextFrame();
}

void CRenderer::BuildCbvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	::ZeroMemory(&cbvHeapDesc, sizeof(cbvHeapDesc));
	cbvHeapDesc.NumDescriptors	= 1 + 2;
	cbvHeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask		= 0;
	HRESULT hResult = m_pD3dDevice->CreateDescriptorHeap(
		&cbvHeapDesc,
		IID_PPV_ARGS(m_pCbvHeap.GetAddressOf())
	);

	if (FAILED(hResult))
		throw std::runtime_error("Failed to Create Cbv Heap");
}

void CRenderer::BuildResourceManager()
{
}
