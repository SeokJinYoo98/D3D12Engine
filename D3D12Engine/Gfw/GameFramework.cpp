#include "Common\pch.h"
#include "GameFramework.h"


CGameFramework::CGameFramework()
{
}

void CGameFramework::InitGame(HINSTANCE hInstance, int nCmdShow, UINT nWidth, UINT nHeight)
{
	CD3DApp::InitWindow(hInstance, nCmdShow, nWidth, nHeight);
	BuildRenderer();
	BuildScene();
	m_gameTimer.Reset();
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

void CGameFramework::ProcessInput()
{
}

void CGameFramework::Update(float fElapsedTime)
{
	if (m_pGameScene)
		m_pGameScene->Update(fElapsedTime);
}

void CGameFramework::Render(float fElapsedTime)
{
	m_pGameRenderer->BeginRendering(DirectX::Colors::AliceBlue);

	if (m_pGameScene)
		m_pGameScene->Render(m_pGameRenderer->GetCommandList());

	m_pGameRenderer->EndRendering();
}

void CGameFramework::BuildScene()
{
	m_pGameRenderer->ResetCommandList();

	// 씬 객체를 생성하고 씬에 포함될 게임 객체들을 생성
	m_pGameScene = std::make_unique<CGameScene>();
	m_pGameScene->InitScene(m_pGameRenderer->GetDevice(), m_pGameRenderer->GetCommandList(), m_pGameRenderer->GetCbvHeap(), m_nClientWidth, m_nClientHeight);

	// 씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가한다.
	m_pGameRenderer->ExcuteCommandList();

	// 그래픽 명령 리스트들이 모두 실행될 때 까지 기다린다.
	m_pGameRenderer->WaitForGpuComplete();

	// 그래픽 리소스들을 생성하는 과정에 생성된 업로드 버퍼들을 소멸시킨다.
	if (m_pGameScene) m_pGameScene->ReleaseUploadBuffers();
}

void CGameFramework::BuildRenderer()
{
	m_pGameRenderer = std::make_unique<CRenderer>();
	m_pGameRenderer->InitRenderer(m_hWnd, m_nClientWidth, m_nClientHeight);
}
