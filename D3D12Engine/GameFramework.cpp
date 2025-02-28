#include "stdafx.h"
#include "GameFramework.h"

void CWindowManager::Initialize(HINSTANCE hInst, HWND hWnd, int width, int height)
{
	m_hInstance = hInst;
	m_hWnd = hWnd;
	RECT rcClient{ };
	::GetClientRect(m_hWnd, &rcClient);
	m_nclientWidth = rcClient.right - rcClient.left;
	m_nclientHeight = rcClient.bottom - rcClient.top;
}

void CWindowManager::OnDestroy()
{
}

void CD3D12DeviceManager::Initialize(const CWindowManager& windowManager)
{
	InitializeDevice();
	InitializeFence();
	InitializeViewportAndScissorRect(windowManager);
	InitializeDescriptorSize();
	Initialize4XMSAA();
	InitializeCommandQueueAndList();
	InitializeSwapChain(windowManager);
	InitializeRtvAndDsvDescriptorHeap();

	CreateRenderTargetView();
	CreateDepthStecilView(windowManager);
}

void CD3D12DeviceManager::OnDestroy()
{
	::CloseHandle(m_hFenceEvent);

	m_pSwapChain->SetFullscreenState(FALSE, nullptr);

#if defined(_DEBUG) 
	IDXGIDebug1 *pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CD3D12DeviceManager::ResetCommandListAndAllocator()
{
	// 명령 할당자와 명령 리스트를 리셋한다.
	HRESULT hResult = m_pCommandAllocator->Reset();
	if (FAILED(hResult)) throw std::runtime_error("Command Alloc reset failed");
	hResult = m_pCommandList->Reset(m_pCommandAllocator.Get(), nullptr);
	if (FAILED(hResult)) throw std::runtime_error("Command List reset failed");
}

void CD3D12DeviceManager::TransitionResourceFromPresentToRenderTarget(D3D12_RESOURCE_BARRIER& d3dResourceBarrier)
{
	/*
		렌더링을 수행하기 전에 스왑 체인 버퍼의 상태를 Present 상태에서 RenderTarget 상태로 전환함으로써,
		GPU가 해당 버퍼에 안전하게 렌더링 작업을 할 수 있도록 보장하는 과정.
	*/
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_pSwapChainBuffers[m_nSwapChainBufferIndex].Get();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}

void CD3D12DeviceManager::SetViewportAndScissorRect()
{
	m_pCommandList->RSSetViewports(1, &m_d3dViewport);
	m_pCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CD3D12DeviceManager::ClearRenderTargetAndDepthStencil(const DirectX::XMVECTOR& xmvClearColor)
{
	// RTV 핸들 계산
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_rtvDescriptorIncrementSize);

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
	DirectX::XMFLOAT4 clearColor;
	DirectX::XMStoreFloat4(&clearColor, xmvClearColor);
	m_pCommandList->ClearRenderTargetView(
		d3dRtvCPUDescriptorHandle,
		reinterpret_cast<const float*>(&clearColor),
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

void CD3D12DeviceManager::TransitionRenderTargetToPresent(D3D12_RESOURCE_BARRIER& d3dResourceBarrier)
{
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	HRESULT hResult = m_pCommandList->Close();
}

void CD3D12DeviceManager::ExcuteCommandList()
{
	ID3D12CommandList* ppd3dCommandLists[] = { m_pCommandList.Get() };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);
}

void CD3D12DeviceManager::WaitForGpuComplete()
{
	UINT64 nFenceValue = m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence.Get(), nFenceValue);

	if (m_pFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CD3D12DeviceManager::PresentSwapChain()
{
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters{ };
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	m_pSwapChain->Present1(1, 0, &dxgiPresentParameters);

	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void CD3D12DeviceManager::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence.Get(), nFenceValue);
	if (m_pFence->GetCompletedValue() < nFenceValue) {
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CD3D12DeviceManager::InitializeDevice()
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
		IID_PPV_ARGS(&m_pD3dDevice)
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
			IID_PPV_ARGS(&m_pD3dDevice)
		);
	}
}

void CD3D12DeviceManager::InitializeFence()
{
	HRESULT hResult = m_pD3dDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_pFence)
	);
	
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*
		펜스와 동기화를 위한 이벤트 객체 생성
		이벤트가 실행되면 이벤트 값을 자동적으로 FALSE가 되도록 생성한다.
	*/
}

// 각 서술자의 크기를 GPU에서 직접 조회하여 저장
// Descirptor Heap내에서 특정 서술자의 위치를 찾을 때 사용
void CD3D12DeviceManager::InitializeDescriptorSize()
{
	m_nRtvDescriptorSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_nDsvDescriptorSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_nCbvSrcDescriptorSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CD3D12DeviceManager::Initialize4XMSAA()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{ };

	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	
	HRESULT hResult = m_pD3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels,
		sizeof(d3dMsaaQualityLevels)
	);

	if (FAILED(hResult))
		throw std::runtime_error("MSAA Failed");
	
	m_nMsaa4xQuality = d3dMsaaQualityLevels.NumQualityLevels;
	
	m_bMsaa4xEnable = (m_nMsaa4xQuality > 1) ? true : false;
}

void CD3D12DeviceManager::InitializeCommandQueueAndList()
{

	D3D12_COMMAND_QUEUE_DESC queueDesc{ };
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_pD3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));

	m_pD3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_pCommandAllocator)
	);

	m_pD3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator.Get(),
		nullptr, // 초기 파이프라인 상태 객체
		IID_PPV_ARGS(m_pCommandList.GetAddressOf())
	);

	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
	m_pCommandList->Close();
}

void CD3D12DeviceManager::InitializeSwapChain(const CWindowManager& wdManager)
{
	m_pSwapChain.Reset();

	RECT rcClient{ };
	::GetClientRect(wdManager.m_hWnd, &rcClient);

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc{ };
	dxgiSwapChainDesc.Width = wdManager.m_nclientWidth;
	dxgiSwapChainDesc.Height = wdManager.m_nclientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQuality - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = nSwapChainBufferCount;
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
		wdManager.m_hWnd,
		&dxgiSwapChainDesc,
		&dxgiSwapChainFullScreenDesc,
		nullptr,
		tempChain.GetAddressOf()
	);

	if (FAILED(tempChain.As(&m_pSwapChain))) {
		throw std::runtime_error("스왑체인 캐스팅 오류");
	}


	m_pFactory->MakeWindowAssociation(wdManager.m_hWnd, DXGI_MWA_NO_ALT_ENTER);;
	// 알트엔터 비활성
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	// 스왑체인 버퍼 인덱스 저장
}

void CD3D12DeviceManager::InitializeRtvAndDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{ };
	rtvHeapDesc.NumDescriptors = nSwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	
	HRESULT hResult = m_pD3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(m_pRtvHeap.GetAddressOf())
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{ };
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	hResult = m_pD3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(m_pDsvHeap.GetAddressOf())
	);

	m_dsvDescriptorIncrementSize =
		m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// 서술자 크기가 다를 수 있으므로, 정확한 크기를 디바이스로부터 얻어야 올바른 메모리 오프셋 계산 가능.
	m_rtvDescriptorIncrementSize =
		m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void CD3D12DeviceManager::InitializeViewportAndScissorRect(const CWindowManager& wdManager)
{
	int width = wdManager.m_nclientWidth;
	int height = wdManager.m_nclientHeight;

	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(width);
	m_d3dViewport.Height = static_cast<float>(height);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	//뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정한다. 
	m_d3dScissorRect = { 0, 0, width, height };
} 

void CD3D12DeviceManager::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle
		= m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < nSwapChainBufferCount; ++i) {
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pSwapChainBuffers[i]));

		m_pD3dDevice->CreateRenderTargetView(
			m_pSwapChainBuffers[i].Get(),
			nullptr,
			d3dRtvCPUDescriptorHandle
		);

		d3dRtvCPUDescriptorHandle.ptr += m_rtvDescriptorIncrementSize;
	}
}

void CD3D12DeviceManager::CreateDepthStecilView(const CWindowManager& wdManager)
{
	D3D12_RESOURCE_DESC d3dResourceDesc{ };
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = wdManager.m_nclientWidth;
	d3dResourceDesc.Height = wdManager.m_nclientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQuality - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*
	GPU 전용 메모리를 사용하고, 
	시스템이 페이지 속성과 메모리 풀을 결정하도록 하며, 
	단일 GPU 환경에서 사용하도록 기본 설정을 한 상태.
	*/
	D3D12_HEAP_PROPERTIES d3dHeapProperties{ };
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue{ };
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pD3dDevice->CreateCommittedResource(
		&d3dHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&d3dClearValue,
		IID_PPV_ARGS(m_pDepthStencilBuffer.GetAddressOf())
	);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_pD3dDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer.Get(),
		nullptr,
		d3dDsvCPUDescriptorHandle
	);
}

CGameFramework::CGameFramework()
	:m_windowManager{ }, m_deviceManager{ }, m_gameTimer{ }, m_pszFrameRate{ }
{
	_tcscpy_s(m_pszFrameRate, _T("YEngine ("));
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_windowManager.Initialize(hInstance, hMainWnd);
	m_deviceManager.Initialize(m_windowManager);

	BuildObjects();

	return true;
}

void CGameFramework::FrameAdvance()
{
	m_gameTimer.Tick(0.0f);

	ProcessInput();
	AnimateObjects();

	//D3D12_RESOURCE_BARRIER d3dResourceBarrier{ };
	//m_deviceManager.ResetCommandListAndAllocator();
	//m_deviceManager.TransitionResourceFromPresentToRenderTarget(d3dResourceBarrier);
	//m_deviceManager.SetViewportAndScissorRect();
	//m_deviceManager.ClearRenderTargetAndDepthStencil(DirectX::Colors::Azure);
	//// 렌더링 코드
	//
	////
	//m_deviceManager.TransitionRenderTargetToPresent(d3dResourceBarrier);
	//m_deviceManager.ExcuteCommandList();
	//m_deviceManager.WaitForGpuComplete();
	//m_deviceManager.PresentSwapChain();

	m_deviceManager.ResetCommandListAndAllocator();
	m_deviceManager.SetViewportAndScissorRect();
	D3D12_RESOURCE_BARRIER d3dResourceBarrier{ };
	m_deviceManager.TransitionResourceFromPresentToRenderTarget(d3dResourceBarrier);
	m_deviceManager.ClearRenderTargetAndDepthStencil(DirectX::Colors::Azure);
	if (m_pScene) m_pScene->Render(m_deviceManager.m_pCommandList);
	m_deviceManager.TransitionRenderTargetToPresent(d3dResourceBarrier);
	m_deviceManager.ExcuteCommandList();

	m_deviceManager.WaitForGpuComplete();

	m_deviceManager.m_pSwapChain->Present(0, 0);

	m_deviceManager.MoveToNextFrame();

	m_gameTimer.GetFrameRate(m_pszFrameRate + 9, 40);
	::SetWindowText(m_windowManager.m_hWnd, m_pszFrameRate);
}

void CGameFramework::OnDestroy()
{
	m_deviceManager.WaitForGpuComplete();
	// ReleaseObjs
	m_deviceManager.OnDestroy();
	m_windowManager.OnDestroy();
	
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::AnimateObjects()
{
}

void CGameFramework::BuildObjects()
{
	auto& pCommandList = m_deviceManager.m_pCommandList;

	pCommandList->Reset(m_deviceManager.m_pCommandAllocator.Get(), nullptr);

	// 씬 객체를 생성하고 씬에 포함될 게임 객체들을 생성
	m_pScene = std::make_unique<CScene>();
	m_pScene->BuildObjects(m_deviceManager.m_pD3dDevice, m_deviceManager.m_pCommandList);

	// 씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다.
	pCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { pCommandList.Get()};
	m_deviceManager.m_pCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	// 그래픽 명령 리스트들이 모두 실행될 때 까지 기다린다.
	m_deviceManager.WaitForGpuComplete();

	// 그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸시킨다.
	if (m_pScene)
		m_pScene->ReleaseUploadBuffers();

	m_gameTimer.Reset();
}
