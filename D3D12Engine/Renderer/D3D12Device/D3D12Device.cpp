#include "Common\pch.h"
#include "D3D12Device.h"

CD3D12Device::CD3D12Device()
{
	m_d3dBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
}

void CD3D12Device::InitDevice(HWND hWnd, UINT nWidth, UINT nHeight)
{
	// COM 함수나 Direct3D 함수 호출의 성공 여부를 나타내는 값.
	HRESULT hResult{ };

	// DXGI 팩토리 생성 시 사용할 플래그들을 저장하는 변수
	UINT dxgiFactoryFlags = 0;

	// 디버그 레이어를 활성화
#if defined (_DEBUG)
	ID3D12Debug* d3dDebugController{ nullptr };
	hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController));
	if (d3dDebugController) {
		d3dDebugController->EnableDebugLayer();
		d3dDebugController->Release();
	}
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	// DXGI 팩토리 생성
	hResult = ::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_pFactory));

	if (FAILED(hResult)) {
		throw std::runtime_error("Failed to create DXGI Factory");
	}

	// 하드웨어 어댑터를 나타내는 장치를 생성해본다.
	hResult = ::D3D12CreateDevice(
		nullptr, // 기본 어댑터 사용하겠다.
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_pDevice)
	);

	if (FAILED(hResult)) {
		// 실패했다면 WARP 어댑터를 나타내는 장치를 생성한다.
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWrapAdapter;
		HRESULT wrapResult = m_pFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWrapAdapter));
		if (FAILED(wrapResult))
			throw std::runtime_error("Faield to Enum Warp Adapter");

		wrapResult = D3D12CreateDevice(
			pWrapAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_pDevice)
		);
	}
	hResult = m_pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_pFence)
	);

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*
		펜스와 동기화를 위한 이벤트 객체 생성
		이벤트가 실행되면 이벤트 값을 자동적으로 FALSE가 되도록 생성한다.
	*/

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{ };

	d3dMsaaQualityLevels.Format				= m_d3dBackBufferFormat;
	d3dMsaaQualityLevels.SampleCount		= 4;
	d3dMsaaQualityLevels.Flags				= D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels	= 0;

	hResult = m_pDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels,
		sizeof(d3dMsaaQualityLevels)
	);

	if (FAILED(hResult))
		throw std::runtime_error("MSAA Failed");

	m_nMsaa4xQuality = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQuality > 1) ? true : false;

	m_nRtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_nDsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_nCbvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateCommandObject();
	CreateSwapChain(hWnd, nWidth, nHeight);
	CreateRtvAndDsvDescriptorHeap();

	CreateRenderTargetView();
	CreateDepthStecilView(hWnd, nWidth, nHeight);
}

void CD3D12Device::CreateSwapChain(HWND hWnd, UINT nWidth, UINT nHeight)
{
	m_pSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc{ };
	dxgiSwapChainDesc.Width = nWidth;
	dxgiSwapChainDesc.Height = nHeight;
	dxgiSwapChainDesc.Format = m_d3dBackBufferFormat;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQuality - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc{ };
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> tempChain{ };
	m_pFactory->CreateSwapChainForHwnd(
		m_pCommandQueue.Get(),
		hWnd,
		&dxgiSwapChainDesc,
		&dxgiSwapChainFullScreenDesc,
		nullptr,
		tempChain.GetAddressOf()
	);

	if (FAILED(tempChain.As(&m_pSwapChain))) {
		throw std::runtime_error("스왑체인 캐스팅 오류");
	}
	m_pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);;
	// 알트엔터 비활성
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	// 스왑체인 버퍼 인덱스 저장
}
void CD3D12Device::CreateRtvAndDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{ };
	rtvHeapDesc.NumDescriptors = SWAP_CHAIN_FRAME_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	HRESULT hResult = m_pDevice->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(m_pRtvHeap.GetAddressOf())
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{ };
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	hResult = m_pDevice->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(m_pDsvHeap.GetAddressOf())
	);
}
void CD3D12Device::CreateCommandObject()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{ };
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));

	m_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_pCommandAllocator)
	);

	m_pDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator.Get(),
		nullptr, // 초기 파이프라인 상태 객체
		IID_PPV_ARGS(m_pCommandList.GetAddressOf())
	);

	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	m_pCommandList->Close();
}

void CD3D12Device::OnResize(HWND hWnd, UINT nWidth, UINT nHeight)
{
	assert(m_pDevice);
	assert(m_pSwapChain);
	assert(m_pCommandAllocator);

	WaitForGpuComplete();
	ResetCommandAlloc();

	for (int i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
		m_pSwapChainBuffers[i].Reset();
	m_pDepthStencilBuffer.Reset();

	m_pSwapChain->ResizeBuffers(
		SWAP_CHAIN_FRAME_COUNT,
		nWidth, nHeight,
		m_d3dBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);

	m_nSwapChainBufferIndex = 0;

	CreateRenderTargetView();
	CreateDepthStecilView(hWnd, nWidth, nHeight);

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_pDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);
	m_pCommandList->ResourceBarrier(
		1,
		&barrier
	);
	ExcuteCommandList();
	WaitForGpuComplete();
}

void CD3D12Device::OnDestroy()
{
	WaitForGpuComplete();
	::CloseHandle(m_hFenceEvent);

	m_pSwapChain->SetFullscreenState(FALSE, nullptr);

#if defined(_DEBUG) 
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CD3D12Device::WaitForGpuComplete()
{
	UINT64 nFenceValue = m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence.Get(), nFenceValue);

	if (m_pFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CD3D12Device::PresentSwapChain()
{
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters{ };
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	m_pSwapChain->Present1(1, 0, &dxgiPresentParameters);

	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void CD3D12Device::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence.Get(), nFenceValue);
	if (m_pFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CD3D12Device::CreateCbvDescriptorHeap(UINT nMesh, UINT nUI, UINT nPass)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	::ZeroMemory(&cbvHeapDesc, sizeof(cbvHeapDesc));

	cbvHeapDesc.NumDescriptors = nMesh + nMesh + nPass;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pDevice->CreateDescriptorHeap(
		&cbvHeapDesc,
		IID_PPV_ARGS(m_pCbvHeap.GetAddressOf())
	);

	if (FAILED(hResult))
		throw std::runtime_error("Failed to Create Cbv Heap");
}


void CD3D12Device::ResetCommandAlloc()
{
	// 명령 할당자와 명령 리스트를 리셋한다.
	HRESULT hResult = m_pCommandAllocator->Reset();
	hResult = m_pCommandList->Reset(m_pCommandAllocator.Get(), nullptr);
}

void CD3D12Device::ResetCommandAlloc(ID3D12CommandAllocator* pCmdAlloc, ID3D12PipelineState* pPso)
{
	HRESULT hResult = pCmdAlloc->Reset();
	if (FAILED(hResult))
		throw std::runtime_error("Failed to reset CmdAlloc");
	hResult = m_pCommandList->Reset(pCmdAlloc, pPso);
	if (FAILED(hResult))
		throw std::runtime_error("Failed to reset CmdList");
}

void CD3D12Device::ExcuteCommandList()
{
	m_pCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pCommandList.Get() };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);
}

void CD3D12Device::ResetCommandList()
{
	m_pCommandList->Reset(
		m_pCommandAllocator.Get(),
		nullptr
	);
}

void CD3D12Device::TransitionResourceFromPresentToRenderTarget()
{
	D3D12_RESOURCE_BARRIER d3dResourceBarrier{ };
	d3dResourceBarrier.Type						= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource		= m_pSwapChainBuffers[m_nSwapChainBufferIndex].Get();
	d3dResourceBarrier.Transition.StateBefore	= D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter	= D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}

void CD3D12Device::ClearRenderTargetAndDepthStencil(const DirectX::XMFLOAT4& xmf4BgColor)
{
	// RTV 핸들 계산
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorSize);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_pCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, FALSE, &d3dDsvCPUDescriptorHandle);

	// 렌더 타겟 뷰와 깊이 스텐실 뷰를 출력-병합 단계(OM)에 연결한다.
	m_pCommandList->OMSetRenderTargets(
		1,
		&d3dRtvCPUDescriptorHandle,
		TRUE,
		&d3dDsvCPUDescriptorHandle
	);

	// 렌더 타겟뷰를 지운다.
	m_pCommandList->ClearRenderTargetView(
		d3dRtvCPUDescriptorHandle,
		&xmf4BgColor.x,
		0,
		nullptr
	);

	// 원하는 값으로 깊이 스텐실 뷰를 지운다.
	m_pCommandList->ClearDepthStencilView(
		d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr
	);
}

void CD3D12Device::TransitionRenderTargetToPresent()
{
	D3D12_RESOURCE_BARRIER d3dResourceBarrier{ };
	d3dResourceBarrier.Type						= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags					= D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource		= m_pSwapChainBuffers[m_nSwapChainBufferIndex].Get();
	d3dResourceBarrier.Transition.StateBefore	= D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter	= D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}

void CD3D12Device::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle
		= m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i) {
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pSwapChainBuffers[i]));

		m_pDevice->CreateRenderTargetView(
			m_pSwapChainBuffers[i].Get(),
			nullptr,
			d3dRtvCPUDescriptorHandle
		);

		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorSize;
	}
}

void CD3D12Device::CreateDepthStecilView(HWND hWnd, UINT nWidth, UINT nHeight)
{
	D3D12_RESOURCE_DESC d3dResourceDesc{ };
	d3dResourceDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment			= 0;
	d3dResourceDesc.Width				= nWidth;
	d3dResourceDesc.Height				= nHeight;
	d3dResourceDesc.DepthOrArraySize	= 1;
	d3dResourceDesc.MipLevels			= 1;
	d3dResourceDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count	= (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality	= (m_bMsaa4xEnable) ? (m_nMsaa4xQuality - 1) : 0;
	d3dResourceDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags				= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*
	GPU 전용 메모리를 사용하고,
	시스템이 페이지 속성과 메모리 풀을 결정하도록 하며,
	단일 GPU 환경에서 사용하도록 기본 설정을 한 상태.
	*/
	D3D12_HEAP_PROPERTIES d3dHeapProperties{ };
	d3dHeapProperties.Type					= D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference	= D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask		= 1;
	d3dHeapProperties.VisibleNodeMask		= 1;

	D3D12_CLEAR_VALUE d3dClearValue{ };
	d3dClearValue.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth	= 1.0f;
	d3dClearValue.DepthStencil.Stencil	= 0;

	m_pDevice->CreateCommittedResource(
		&d3dHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&d3dClearValue,
		IID_PPV_ARGS(m_pDepthStencilBuffer.GetAddressOf())
	);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer.Get(),
		nullptr,
		d3dDsvCPUDescriptorHandle
	);
}
