#pragma once
#include "Core\D3D12DeviceManager.h"
#include "GameTimer.h"
#include "Scene.h"
#include "Camera.h"

class CGameFramework
{
public:
	CGameFramework();


public:
	// 외부 호출
	bool Initialize(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void FrameAdvance(HWND hWnd);
	
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void PauseGame();
	void StartGame();

	void OnResize(HWND hWnd);

private:
	// 게임 관련
	void ProcessInput();
	void AnimateObjects();
	void Render();

	void BuildObjects();

private:

private:
	CD3D12DeviceManager	m_deviceManager;

	std::unique_ptr<CScene>		m_pScene;
	std::unique_ptr<CCamera>	m_pCamera;

	CGameTimer			m_gameTimer;
	_TCHAR				m_pszFrameRate[50];

private:
	bool m_bPauseGame{ false };

	UINT m_nWidth{ 0 };
	UINT m_nHeight{ 0 };

	POINT m_nLastMousePos;
};

