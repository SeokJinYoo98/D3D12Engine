#pragma once
#include "Gfw\GameFramework.h"

class CD3DApp
{
public:
	CD3DApp(HINSTANCE hInstance, int nCmdShow);
	~CD3DApp();

public:
	BOOL Initialize(INT nBufferWidth, INT nBufferHeight);
	int Run();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	ATOM MyRegisterClass(HINSTANCE hInstance) const;

private:
	static constexpr int MAX_LOADSTRING{ 100 };

	HINSTANCE m_hInstance;      // WinMain으로부터 받은 인스턴스 핸들
	HWND      m_hWnd;           // 생성된 창의 핸들
	int       m_nCmdShow;       // 창 표시 형식

	// 리소스나 문자열 테이블에서 사용할 인스턴스
	HINSTANCE m_hAppInstance;

	// 창의 제목, 클래스 이름 등을 저장할 버퍼
	WCHAR m_szTitle[MAX_LOADSTRING];
	WCHAR m_szWindowClass[MAX_LOADSTRING];

	CGameFramework m_gameFramework;

	bool m_bPauseApp{ false };
};

