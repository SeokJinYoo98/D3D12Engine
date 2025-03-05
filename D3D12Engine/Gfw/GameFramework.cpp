#include "Common\pch.h"
#include "GameFramework.h"


CGameFramework::CGameFramework()
	: m_deviceManager{ }, m_gameTimer{ }, m_pszFrameRate{ }
{
	_tcscpy_s(m_pszFrameRate, _T("YEngine ("));
}

bool CGameFramework::Initialize(HINSTANCE hInstance, HWND hMainWnd)
{
	RECT rcClient; ::GetClientRect(hMainWnd, &rcClient);
	m_nWidth = rcClient.right - rcClient.left;
	m_nHeight = rcClient.bottom - rcClient.top;

	m_deviceManager.Initialize(hMainWnd, m_nWidth, m_nHeight);

	BuildObjects();

	return true;
}

void CGameFramework::FrameAdvance(HWND hWnd)
{
	m_gameTimer.Tick(0.0f);

	ProcessInput();
	AnimateObjects();

	m_deviceManager.ResetCommandListAndAllocator();
	auto d3dResourceBarrier = m_deviceManager.TransitionResourceFromPresentToRenderTarget();
	m_deviceManager.ClearRenderTargetAndDepthStencil(DirectX::Colors::Azure);
	
	Render();

	m_deviceManager.TransitionRenderTargetToPresent(d3dResourceBarrier);
	m_deviceManager.ExcuteCommandList();

	m_deviceManager.WaitForGpuComplete();

	m_deviceManager.m_pSwapChain->Present(0, 0);

	m_deviceManager.MoveToNextFrame();

	m_gameTimer.GetFrameRate(m_pszFrameRate + 9, 40);
	::SetWindowText(hWnd, m_pszFrameRate);
}

bool CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CGameFramework::PauseGame()
{
	if (m_bPauseGame) return;

	m_bPauseGame = true;
	m_gameTimer.Stop();
}

void CGameFramework::StartGame()
{
	if (!m_bPauseGame) return;

	m_bPauseGame = false;
	m_gameTimer.Start();
}

void CGameFramework::OnResize(HWND hWnd)
{
	RECT rcClient; ::GetClientRect(hWnd, &rcClient);
	m_nWidth = rcClient.right - rcClient.left;
	m_nHeight = rcClient.bottom - rcClient.top;

	m_deviceManager.OnResize();
	m_pScene->OnResize(m_nWidth, m_nHeight);
}

void CGameFramework::OnDestroy()
{
	m_deviceManager.WaitForGpuComplete();
	m_deviceManager.OnDestroy();
}

void CGameFramework::Render()
{
	if (m_pScene) 
		m_pScene->Render(m_deviceManager.m_pCommandList.Get(), m_pCamera.get());
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_gameTimer.GetTimeElapsed());
}

void CGameFramework::BuildObjects()
{
	auto& pCommandList = m_deviceManager.m_pCommandList;

	pCommandList->Reset(m_deviceManager.m_pCommandAllocator.Get(), nullptr);

	// ī�޶� ��ü�� �����ϰ�, ����Ʈ, ����, ����-�� ��ȯ ����� �����ϰ� ����
	m_pCamera = std::make_unique<CCamera>();
	m_pCamera->SetViewport(0, 0, m_nWidth, m_nHeight, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, m_nWidth, m_nHeight);
	m_pCamera->GenerateProjectionMatrix(1.0f, 500.f, float(m_nWidth) / float(m_nHeight), 90.f);
	m_pCamera->GenerateViewMatrix(DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	// �� ��ü�� �����ϰ� ���� ���Ե� ���� ��ü���� ����
	m_pScene = std::make_unique<CScene>();
	m_pScene->BuildObjects(m_deviceManager.m_pD3dDevice.Get(), m_deviceManager.m_pCommandList.Get());

	// �� ��ü�� �����ϱ� ���Ͽ� �ʿ��� �׷��� ��� ����Ʈ���� ��� ť�� �߰��Ѵ�.
	pCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { pCommandList.Get()};
	m_deviceManager.m_pCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	// �׷��� ��� ����Ʈ���� ��� ����� �� ���� ��ٸ���.
	m_deviceManager.WaitForGpuComplete();

	// �׷��� ���ҽ����� �����ϴ� ������ ������ ���ε� ���۵��� �Ҹ��Ų��.
	if (m_pScene) m_pScene->ReleaseUploadBuffers();

	m_gameTimer.Reset();
}
