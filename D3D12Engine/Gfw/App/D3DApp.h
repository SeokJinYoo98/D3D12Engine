#pragma once
#include "GameTimer.h"
class CD3DApp
{
public:
	CD3DApp() { }
	CD3DApp(const CD3DApp& rhs) = delete;
	CD3DApp& operator=(const CD3DApp& rhs) = delete;
	virtual ~CD3DApp();

private:
	static LRESULT CALLBACK		WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL						MyRegisterClass(HINSTANCE hInstance) const;
	void						FrameAdvance(float fElapsedTime);
	void						CalculateFrameRate();

public:
	int							Run();
	virtual void				OnDestroy();
	
protected:
	BOOL						InitWindow(HINSTANCE hInstance, int nCmdShow, UINT nWidth = 800, UINT nHeight = 600);
	float						AspectRatio();

	virtual LRESULT				MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void				UpdateClientRect();
	virtual void				OnResize();
	
	virtual void				ProcessInput()			   { }
	virtual void				Render(float fElapsedTime) { }
	virtual void				Update(float fElapsedTime) { }

	// Convenience overrides for handling mouse input.
	virtual void				OnMouseDown(WPARAM btnState, int x, int y)		{ }
	virtual void				OnMouseUp(WPARAM btnState, int x, int y)		{ }
	virtual void				OnMouseMove(WPARAM btnState, int x, int y)		{ }

	virtual void				OnKeyboardDown(WPARAM wParam, LPARAM lParam)	{ }
	virtual void				OnKeyboardUp(WPARAM wParam, LPARAM lParam)		{ }
protected:
	static constexpr int		MAX_LOADSTRING{ 100 };

	HINSTANCE					m_hInstance		= nullptr;      // WinMain으로부터 받은 인스턴스 핸들
	HWND						m_hWnd			= nullptr;      // 생성된 창의 핸들
	int							m_nCmdShow		= 0;			// 창 표시 형식

	// 창의 제목, 클래스 이름 등을 저장할 버퍼
	WCHAR						m_szTitle[MAX_LOADSTRING]		= { };
	WCHAR						m_szWindowClass[MAX_LOADSTRING] = { };

	UINT						m_nClientWidth		= 0;
	UINT						m_nClientHeight		= 0;

	bool						m_bAppPaused		= false;
	bool						m_bMinimized		= false;
	bool						m_bMaximized		= false;
	bool						m_bResizing			= false;
	bool						m_bFullscreenState	= false;

	CGameTimer					m_gameTimer			= { };
	_TCHAR						m_pszFrameRate[50]	= { };
};

