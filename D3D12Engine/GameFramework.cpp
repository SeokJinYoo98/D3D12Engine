#include "stdafx.h"
#include "GameFramework.h"

void WindowManager::Initialize(HINSTANCE hInst, HWND hWnd, int width, int height)
{
	if (CONSOLE_MESSAGE)
		std::cout << "WD Manager On Create\n";
	m_hInstance = hInst;
	m_hWnd = hWnd;
	RECT rcClient{ };
	::GetClientRect(m_hWnd, &rcClient);
	m_clientWidth = rcClient.right - rcClient.left;
	m_clientHeight = rcClient.bottom - rcClient.top;
}

void WindowManager::OnDestroy()
{
}

void D3D12DeviceManager::Initialize(const WindowManager& windowManager)
{
	if (CONSOLE_MESSAGE)
		std::cout << "Device On Create\n";
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

void D3D12DeviceManager::OnDestroy()
{
	::CloseHandle(m_fenceEvent);

	m_swapChain->SetFullscreenState(FALSE, nullptr);

#if defined(_DEBUG) 
	IDXGIDebug1 *pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void D3D12DeviceManager::ResetCommandListAndAllocator()
{
	// ��� �Ҵ��ڿ� ��� ����Ʈ�� �����Ѵ�.
	HRESULT hResult = m_directCmdListAlloc->Reset();
	if (FAILED(hResult)) throw std::runtime_error("Command Alloc reset failed");
	hResult = m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr);
	if (FAILED(hResult)) throw std::runtime_error("Command List reset failed");
}

void D3D12DeviceManager::TransitionResourceFromPresentToRenderTarget(D3D12_RESOURCE_BARRIER& d3dResourceBarrier)
{
	/*
		�������� �����ϱ� ���� ���� ü�� ������ ���¸� Present ���¿��� RenderTarget ���·� ��ȯ�����ν�,
		GPU�� �ش� ���ۿ� �����ϰ� ������ �۾��� �� �� �ֵ��� �����ϴ� ����.
	*/
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_swapChainBuffer[m_swapChainBufferIndex].Get();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_commandList->ResourceBarrier(1, &d3dResourceBarrier);
}

void D3D12DeviceManager::SetViewportAndScissorRect()
{
	m_commandList->RSSetViewports(1, &m_d3dViewport);
	m_commandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void D3D12DeviceManager::ClearRenderTargetAndDepthStencil(const DirectX::XMVECTOR& xmvClearColor)
{
	// RTV �ڵ� ���
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_swapChainBufferIndex * m_rtvDescriptorIncrementSize);

	// ���� Ÿ�ٺ並 �����.
	DirectX::XMFLOAT4 clearColor;
	DirectX::XMStoreFloat4(&clearColor, xmvClearColor);
	m_commandList->ClearRenderTargetView(
		d3dRtvCPUDescriptorHandle,
		reinterpret_cast<const float*>(&clearColor),
		0,
		nullptr
	);


	// ����-���ٽ� �������� CPU �ּҸ� ��� 
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

	// ���ϴ� ������ ���� ���ٽ� �並 �����.
	m_commandList->ClearDepthStencilView(
		d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr
	);

	// ���� Ÿ�� ��� ���� ���ٽ� �並 ���-���� �ܰ�(OM)�� �����Ѵ�.
	m_commandList->OMSetRenderTargets(
		1,
		&d3dRtvCPUDescriptorHandle,
		TRUE,
		&d3dDsvCPUDescriptorHandle
	);
	
}

void D3D12DeviceManager::TransitionRenderTargetToPresent(D3D12_RESOURCE_BARRIER& d3dResourceBarrier)
{
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_commandList->ResourceBarrier(1, &d3dResourceBarrier);

	HRESULT hResult = m_commandList->Close();
}

void D3D12DeviceManager::ExcuteCommandList()
{
	ID3D12CommandList* ppd3dCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
}

void D3D12DeviceManager::WaitForGpuComplete()
{
	// CPU �潺�� ���� ������Ų��.
	m_fenceValue++;
	
	// GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	const UINT64 nFence = m_fenceValue;
	HRESULT hResult = m_commandQueue->Signal(m_d3dFence.Get(), nFence);

	// �潺�� ���� ���� ������ ������ ������ ������ ���� �� ������ ��ٸ���.
	if (m_d3dFence->GetCompletedValue() < nFence) {
		hResult = m_d3dFence->SetEventOnCompletion(nFence, m_fenceEvent);
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void D3D12DeviceManager::PresentSwapChain()
{
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters{ };
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	m_swapChain->Present1(1, 0, &dxgiPresentParameters);

	m_swapChainBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void D3D12DeviceManager::InitializeDevice()
{
	if (CONSOLE_MESSAGE)
		std::cout << "InitDevice\n";
	// COM �Լ��� Direct3D �Լ� ȣ���� ���� ���θ� ��Ÿ���� ��.
	HRESULT hResult{ };

	// DXGI ���丮 ���� �� ����� �÷��׵��� �����ϴ� ����
	UINT dxgiFactoryFlags = 0;

	// ����� ���̾ Ȱ��ȭ
#if defined (_DEBUG)
	ID3D12Debug* d3dDebugController{ nullptr };
	hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController));
	if (d3dDebugController) {
		d3dDebugController->EnableDebugLayer();
		d3dDebugController->Release();
	}
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	// DXGI ���丮 ����
	hResult = ::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory));

	if (FAILED(hResult)) {
		throw std::runtime_error("Failed to create DXGI Factory");
	}

	// �ϵ���� ����͸� ��Ÿ���� ��ġ�� �����غ���.
	hResult = ::D3D12CreateDevice(
		nullptr, // �⺻ ����� ����ϰڴ�.
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_d3dDevice)
	);

	if (FAILED(hResult)) {
		// �����ߴٸ� WARP ����͸� ��Ÿ���� ��ġ�� �����Ѵ�.
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWrapAdapter;
		HRESULT wrapResult = m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWrapAdapter));
		if (FAILED(wrapResult))
			throw std::runtime_error("Faield to Enum Warp Adapter");

		wrapResult = D3D12CreateDevice(
			pWrapAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_d3dDevice)
		);
	}
	if (CONSOLE_MESSAGE)
		std::cout << "InitDevice End\n";
}

void D3D12DeviceManager::InitializeFence()
{
	if (CONSOLE_MESSAGE)
		std::cout << "Init Fence\n";
	HRESULT hResult = m_d3dDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_d3dFence)
	);
	m_fenceValue = 0;
	
	m_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*
		�潺�� ����ȭ�� ���� �̺�Ʈ ��ü ����
		�̺�Ʈ�� ����Ǹ� �̺�Ʈ ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.
	*/
	if (CONSOLE_MESSAGE)
		std::cout << "Init Fence end\n";
}

// �� �������� ũ�⸦ GPU���� ���� ��ȸ�Ͽ� ����
// Descirptor Heap������ Ư�� �������� ��ġ�� ã�� �� ���
void D3D12DeviceManager::InitializeDescriptorSize()
{
	if (CONSOLE_MESSAGE)
		std::cout << "InitializeDescriptorSize\n";
	m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrcDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeDescriptorSize end\n";
}

void D3D12DeviceManager::Initialize4XMSAA()
{
	if (CONSOLE_MESSAGE)
		std::cout << "Initialize4XMSAA\n";
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{ };

	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	
	HRESULT hResult = m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels,
		sizeof(d3dMsaaQualityLevels)
	);

	if (FAILED(hResult))
		throw std::runtime_error("MSAA Failed");
	
	m_4xMsaaQuality = d3dMsaaQualityLevels.NumQualityLevels;
	
	m_4xMsaaEnable = (m_4xMsaaQuality > 1) ? true : false;
	if (CONSOLE_MESSAGE)
		std::cout << "Initialize4XMSAA end\n";
}

void D3D12DeviceManager::InitializeCommandQueueAndList()
{
	if (CONSOLE_MESSAGE)
		std::cout << "InitializeCommandQueueAndList\n";

	D3D12_COMMAND_QUEUE_DESC queueDesc{ };
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

	m_d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_directCmdListAlloc)
	);

	m_d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_directCmdListAlloc.Get(),
		nullptr, // �ʱ� ���������� ���� ��ü
		IID_PPV_ARGS(m_commandList.GetAddressOf())
	);

	//��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����.
	m_commandList->Close();

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeCommandQueueAndList end\n";
}

void D3D12DeviceManager::InitializeSwapChain(const WindowManager& wdManager)
{
	if (CONSOLE_MESSAGE)
		std::cout << "InitializeSwapChain\n";

	m_swapChain.Reset();

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeSwapChain1\n";

	RECT rcClient{ };
	::GetClientRect(wdManager.m_hWnd, &rcClient);

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeSwapChain2\n";

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc{ };
	dxgiSwapChainDesc.Width = wdManager.m_clientWidth;
	dxgiSwapChainDesc.Height = wdManager.m_clientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_4xMsaaEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_4xMsaaEnable) ? (m_4xMsaaQuality - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = SwapChainBufferCount;
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
	m_dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		wdManager.m_hWnd,
		&dxgiSwapChainDesc,
		&dxgiSwapChainFullScreenDesc,
		nullptr,
		tempChain.GetAddressOf()
	);
	

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeSwapChain3\n";

	if (FAILED(tempChain.As(&m_swapChain))) {
		throw std::runtime_error("����ü�� ĳ���� ����");
	}

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeSwapChain4\n";

	m_dxgiFactory->MakeWindowAssociation(wdManager.m_hWnd, DXGI_MWA_NO_ALT_ENTER);;
	// ��Ʈ���� ��Ȱ��
	m_swapChainBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	// ����ü�� ���� �ε��� ����

	if (CONSOLE_MESSAGE)
		std::cout << "InitializeSwapChain end\n";

}

void D3D12DeviceManager::InitializeRtvAndDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{ };
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	
	HRESULT hResult = m_d3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(m_rtvHeap.GetAddressOf())
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{ };
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	hResult = m_d3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(m_dsvHeap.GetAddressOf())
	);

	m_dsvDescriptorIncrementSize =
		m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// ������ ũ�Ⱑ �ٸ� �� �����Ƿ�, ��Ȯ�� ũ�⸦ ����̽��κ��� ���� �ùٸ� �޸� ������ ��� ����.
	m_rtvDescriptorIncrementSize =
		m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void D3D12DeviceManager::InitializeViewportAndScissorRect(const WindowManager& wdManager)
{
	if (CONSOLE_MESSAGE)
		std::cout << "InitializeViewportAndScissorRect\n";
	int width = wdManager.m_clientWidth;
	int height = wdManager.m_clientHeight;

	m_d3dViewport.TopLeftX = 0;
	m_d3dViewport.TopLeftY = 0;
	m_d3dViewport.Width = static_cast<float>(width);
	m_d3dViewport.Height = static_cast<float>(height);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	//����Ʈ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�. 
	m_d3dScissorRect = { 0, 0, width, height };
	if (CONSOLE_MESSAGE)
		std::cout << "InitializeViewportAndScissorRect end\n";
} 

void D3D12DeviceManager::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle
		= m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < SwapChainBufferCount; ++i) {
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i]));

		m_d3dDevice->CreateRenderTargetView(
			m_swapChainBuffer[i].Get(),
			nullptr,
			d3dRtvCPUDescriptorHandle
		);

		d3dRtvCPUDescriptorHandle.ptr += m_rtvDescriptorIncrementSize;
	}
}

void D3D12DeviceManager::CreateDepthStecilView(const WindowManager& wdManager)
{
	D3D12_RESOURCE_DESC d3dResourceDesc{ };
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = wdManager.m_clientWidth;
	d3dResourceDesc.Height = wdManager.m_clientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_4xMsaaEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_4xMsaaEnable) ? (m_4xMsaaQuality - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*
	GPU ���� �޸𸮸� ����ϰ�, 
	�ý����� ������ �Ӽ��� �޸� Ǯ�� �����ϵ��� �ϸ�, 
	���� GPU ȯ�濡�� ����ϵ��� �⺻ ������ �� ����.
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

	m_d3dDevice->CreateCommittedResource(
		&d3dHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&d3dClearValue,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())
	);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dDevice->CreateDepthStencilView(
		m_depthStencilBuffer.Get(),
		nullptr,
		d3dDsvCPUDescriptorHandle
	);
}

GameFramework::GameFramework()
	:m_windowManager{ }, m_deviceManager{ }
{
}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	if (CONSOLE_MESSAGE) 
		std::cout << "GFW On Create\n";
	
	m_windowManager.Initialize(hInstance, hMainWnd);
	m_deviceManager.Initialize(m_windowManager);

	BuildObjects();

	if (CONSOLE_MESSAGE)
		std::cout << "GFW On Create end\n";
	return true;
}

void GameFramework::FrameAdvance()
{
	ProcessInput();
	AnimateObjects();
	D3D12_RESOURCE_BARRIER d3dResourceBarrier{ };
	m_deviceManager.ResetCommandListAndAllocator();
	m_deviceManager.TransitionResourceFromPresentToRenderTarget(d3dResourceBarrier);
	m_deviceManager.SetViewportAndScissorRect();
	m_deviceManager.ClearRenderTargetAndDepthStencil(DirectX::Colors::Azure);
	
	// ������ �ڵ�
	
	//

	m_deviceManager.TransitionRenderTargetToPresent(d3dResourceBarrier);
	m_deviceManager.ExcuteCommandList();
	m_deviceManager.WaitForGpuComplete();
	m_deviceManager.PresentSwapChain();
}

void GameFramework::OnDestroy()
{
	m_deviceManager.WaitForGpuComplete();
	// ReleaseObjs
	m_deviceManager.OnDestroy();
	m_windowManager.OnDestroy();
	
}

void GameFramework::ProcessInput()
{
}

void GameFramework::AnimateObjects()
{
}

void GameFramework::BuildObjects()
{
}
