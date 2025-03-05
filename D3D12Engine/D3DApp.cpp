#include "Common\pch.h"
#include "Resources\Resource.h"
#include "D3DApp.h"

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // D3DApp �ν��Ͻ� ����
    CD3DApp app(hInstance, nCmdShow);

    // â �ʱ�ȭ
    if (!app.Initialize(800, 600))
    {
        return -1;
    }

    // �޽��� ���� ����
    return app.Run();
}

CD3DApp::CD3DApp(HINSTANCE hInstance, int nCmdShow)
    : m_hInstance(hInstance)
    , m_hWnd(nullptr)
    , m_nCmdShow(nCmdShow)
    , m_hAppInstance(hInstance)
{
    // ���ڿ� ���̺��� â ����, Ŭ���� �̸� ���� �ҷ��� �� �ִٸ� ��� (resource.h, .rc �ʿ�)
    LoadStringW(m_hAppInstance, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
    LoadStringW(m_hAppInstance, IDC_D3D12ENGINE, m_szWindowClass, MAX_LOADSTRING);
}

CD3DApp::~CD3DApp()
{
}

BOOL CD3DApp::Initialize(INT nBufferWidth, INT nBufferHeight)
{
    // 1. â Ŭ���� ���
    if (!MyRegisterClass(m_hInstance)) return false;

    RECT rc = { 0, 0, nBufferWidth, nBufferHeight };
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
    AdjustWindowRect(&rc, dwStyle, FALSE);

    // 2. â ����
    m_hWnd = CreateWindowEx(
        0,
        m_szWindowClass,     // ����� â Ŭ���� �̸�
        m_szTitle,           // â ����
        WS_OVERLAPPEDWINDOW, // â ��Ÿ��
        CW_USEDEFAULT,       // X ��ǥ
        CW_USEDEFAULT,       // Y ��ǥ
        rc.right - rc.left,  // �ʺ�
        rc.bottom - rc.top,  // ����
        nullptr,             // �θ� â
        nullptr,             // �޴�
        m_hInstance,         // �ν��Ͻ� �ڵ�
        this                 // lpParam: �� Ŭ������ ����
    );

    if (!m_hWnd) return false;

    m_gameFramework.Initialize(m_hInstance, m_hWnd);

    ::ShowWindow(m_hWnd, m_nCmdShow);
    ::UpdateWindow(m_hWnd);

    return true;
}

int CD3DApp::Run() 
{
    HACCEL hAccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_D3D12ENGINE));
    MSG msg{ };
    // WM_QUIT�� �߻��� ������ �ݺ�
	while (true)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else {
			m_gameFramework.FrameAdvance(m_hWnd);
		}
	}
    return static_cast<int>(msg.wParam);
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
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->m_hWnd = hWnd; // ��� ������ hWnd ����
        }
    }
    else
    {
        pThis = reinterpret_cast<CD3DApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        return pThis->HandleMessage(hWnd, message, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT CD3DApp::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        m_gameFramework.OnProcessingKeyboardMessage(hWnd, message, wParam, lParam);
        return 0;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
        m_gameFramework.OnProcessingMouseMessage(hWnd, message, wParam, lParam);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

ATOM CD3DApp::MyRegisterClass(HINSTANCE hInstance) const
{
    WNDCLASSEX wcex { };
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = CD3DApp::WndProc; // ���� �Լ�
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName  = m_szWindowClass;
    // �������� ����Ϸ���, resource.h�� .rc ���Ͽ��� ������ ID�� �����ؾ� ��
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D12ENGINE));
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}
