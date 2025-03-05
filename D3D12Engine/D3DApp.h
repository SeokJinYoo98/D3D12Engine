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

	HINSTANCE m_hInstance;      // WinMain���κ��� ���� �ν��Ͻ� �ڵ�
	HWND      m_hWnd;           // ������ â�� �ڵ�
	int       m_nCmdShow;       // â ǥ�� ����

	// ���ҽ��� ���ڿ� ���̺��� ����� �ν��Ͻ�
	HINSTANCE m_hAppInstance;

	// â�� ����, Ŭ���� �̸� ���� ������ ����
	WCHAR m_szTitle[MAX_LOADSTRING];
	WCHAR m_szWindowClass[MAX_LOADSTRING];

	CGameFramework m_gameFramework;

	bool m_bPauseApp{ false };
};

