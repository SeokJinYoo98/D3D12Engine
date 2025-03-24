#include "Common\pch.h"
#include "Resources\Resource.h"
#include "D3DApp.h"

CD3DApp::~CD3DApp()
{
}

int CD3DApp::Run() 
{
    MSG msg = { 0 };
    
    m_gameTimer.Reset();

	while (msg.message != WM_QUIT)
	{
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else {
            m_gameTimer.Tick();
            if (!m_bAppPaused)
                FrameAdvance(m_gameTimer.GetTimeElapsed());
            else
                Sleep(100);
        }
	}

    return static_cast<int>(msg.wParam);
}

void CD3DApp::OnDestroy()
{
    UnregisterClass(m_szWindowClass, m_hInstance);
}

LRESULT CD3DApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CD3DApp* pThis = nullptr;

    if (message == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<CD3DApp*>(pCreate->lpCreateParams);
        if (pThis)
        {
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            pThis->m_hWnd = hWnd;
        }
    }
    else
    {
        pThis = reinterpret_cast<CD3DApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->MsgProc(hWnd, message, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT CD3DApp::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            m_bAppPaused = true;
            m_gameTimer.Stop();
        }
        else
        {
            m_bAppPaused = false;
            m_gameTimer.Start();
        }
        return 0;

        // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        // Save the new client area dimensions.
        m_nClientWidth = LOWORD(lParam);
        m_nClientHeight = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED)
		{
			m_bAppPaused = true;
			m_bMinimized = true;
			m_bMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_bAppPaused = false;
			m_bMinimized = false;
			m_bMaximized = true;
			OnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{

			// Restoring from minimized state?
			if (m_bMinimized)
			{
				m_bAppPaused = false;
				m_bMinimized = false;
				OnResize();
			}

			// Restoring from maximized state?
			else if (m_bMaximized)
			{
				m_bAppPaused = false;
				m_bMaximized = false;
				OnResize();
			}
			else if (m_bResizing)
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				OnResize();
			}
		}
        
        return 0;

        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
        m_bAppPaused = true;
        m_bResizing = true;
        m_gameTimer.Stop();
        return 0;

        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
        m_bAppPaused = false;
        m_bResizing = false;
        m_gameTimer.Start();
        OnResize();
        return 0;

        // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
        OnDestroy();
        PostQuitMessage(0);
        return 0;

        // The WM_MENUCHAR message is sent when a menu is active and the user presses 
        // a key that does not correspond to any mnemonic or accelerator key. 
    case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

        // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
        return 0;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        OnKeyboardDown(wParam, lParam);
        return 0;
    case WM_SYSKEYUP:
    case WM_KEYUP:
        OnKeyboardUp(wParam, lParam);
        return 0;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void CD3DApp::FrameAdvance(float fElapsedTime)
{
    ProcessInput(fElapsedTime);
    Update(fElapsedTime);
    Render(fElapsedTime);
    CalculateFrameRate();
}

void CD3DApp::CalculateFrameRate()
{
    m_gameTimer.GetFrameRate(m_pszFrameRate + 9, 40);
    ::SetWindowText(m_hWnd, m_pszFrameRate);
}

BOOL CD3DApp::InitWindow(HINSTANCE hInstance, int nCmdShow, UINT nWidth, UINT nHeight)
{
    m_hInstance = hInstance;
    m_nCmdShow = nCmdShow;
    m_nClientHeight = nHeight; m_nClientWidth = nWidth;

    LoadStringW(m_hInstance, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
    LoadStringW(m_hInstance, IDC_D3D12ENGINE, m_szWindowClass, MAX_LOADSTRING);
    _tcscpy_s(m_pszFrameRate, _T("YEngine ("));

    // 1. 창 클래스 등록
    if (!MyRegisterClass(m_hInstance)) return false;

    RECT rc = { 0, 0, static_cast<LONG>(m_nClientWidth), static_cast<LONG>(m_nClientHeight) };
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
    AdjustWindowRect(&rc, dwStyle, FALSE);

    // 2. 창 생성
    m_hWnd = CreateWindowEx(
        0,
        m_szWindowClass,        // 등록한 창 클래스 이름
        m_szTitle,              // 창 제목
        WS_OVERLAPPEDWINDOW,    // 창 스타일
        CW_USEDEFAULT,          // X 좌표
        CW_USEDEFAULT,          // Y 좌표
        m_nClientWidth,         // 너비
        m_nClientHeight,        // 높이
        nullptr,                // 부모 창
        nullptr,                // 메뉴
        m_hInstance,            // 인스턴스 핸들
        this                    // lpParam: 이 클래스를 전달
    );

    if (!m_hWnd) return false;

    ::ShowWindow(m_hWnd, m_nCmdShow);
    ::UpdateWindow(m_hWnd);

    return true;
}

BOOL CD3DApp::MyRegisterClass(HINSTANCE hInstance) const
{
    WNDCLASSEX wcex { };
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = CD3DApp::WndProc; // 정적 함수
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName  = m_szWindowClass;
    // 아이콘을 사용하려면, resource.h와 .rc 파일에서 아이콘 ID를 정의해야 함
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D12ENGINE));
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

void CD3DApp::UpdateClientRect()
{
    RECT rcClient; ::GetClientRect(m_hWnd, &rcClient);
    m_nClientWidth = rcClient.right - rcClient.left;
    m_nClientHeight = rcClient.bottom - rcClient.top;
}

void CD3DApp::OnResize()
{
    UpdateClientRect();
}

void CD3DApp::OnKeyboardDown(WPARAM wParam, LPARAM lParam)
{
    if (wParam < 256)
    {
        m_keyState[wParam] |= 0x80;
    }
}

void CD3DApp::OnKeyboardUp(WPARAM wParam, LPARAM lParam)
{
    if (wParam < 256)
    {
        m_keyState[wParam] &= ~0x80;
    }
}

float CD3DApp::AspectRatio()
{
    return static_cast<float>(m_nClientWidth) / m_nClientHeight;
}
