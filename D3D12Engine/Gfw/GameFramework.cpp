#include "Common\pch.h"
#include "GameFramework.h"


CGameFramework::CGameFramework()
{
}

void CGameFramework::InitGame(HINSTANCE hInstance, int nCmdShow, UINT nWidth, UINT nHeight)
{
	CD3DApp::InitWindow(hInstance, nCmdShow, nWidth, nHeight);
	BuildRenderer();
	//BuildScene();
	m_gameTimer.Reset();
}

void CGameFramework::StartScene(std::unique_ptr<CGameScene> pNewScene)
{
	m_pGameScene = std::move(pNewScene);
	BuildScene();
}

void CGameFramework::OnResize()
{
	CD3DApp::OnResize();
	if (m_pGameRenderer)
		m_pGameRenderer->OnResize(m_hWnd, m_nClientWidth, m_nClientHeight);
	if (m_pGameScene)
		m_pGameScene->OnResize(m_nClientWidth, m_nClientHeight);
}

void CGameFramework::OnDestroy()
{
	m_pGameRenderer->OnDestroy();
	CD3DApp::OnDestroy();
	m_pGameScene->OnDestroy();
}
void CGameFramework::ProcessInput(float fElapsedTime)
{
	if (m_pGameScene)
		m_pGameScene->ProcessInput(m_keyState, fElapsedTime);
}

void CGameFramework::Update(float fElapsedTime)
{
	if (m_pGameScene) {
		m_pGameScene->Update(fElapsedTime);
		m_pGameRenderer->Update(m_pGameScene.get());
	}
}

void CGameFramework::Render(float fElapsedTime)
{
	if (m_pGameScene)
	{
		m_pGameRenderer->Render(m_pGameScene.get(), m_bDrawWorldGrid);
	}
}

void CGameFramework::BuildScene()
{
	//m_pGameScene = std::make_unique<CGameScene>();
	m_pGameScene->InitScene(m_nClientWidth, m_nClientHeight);
}

void CGameFramework::BuildRenderer()
{
	m_pGameRenderer = std::make_unique<CRenderer>();
	m_pGameRenderer->InitRenderer(m_hWnd, m_nClientWidth, m_nClientHeight);
}
