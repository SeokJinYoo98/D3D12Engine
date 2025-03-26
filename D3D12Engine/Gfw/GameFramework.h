#pragma once
#include "Gfw/App/D3DApp.h"
#include "Gfw/GameScene/GameScene.h"
#include "Renderer/Renderer.h"


class CGameFramework : public CD3DApp {
public:
	CGameFramework();
	CGameFramework(const CGameFramework& rhs) = delete;
	CGameFramework& operator=(const CGameFramework& rhs) = delete;
	~CGameFramework() { }
public:
	virtual void InitGame(HINSTANCE hInstance, int nCmdShow, UINT nWidth = 800, UINT nHeight = 600);

private:
	void OnResize() override;
	void OnDestroy() override;

private:
	void ProcessInput(float fElapsedTime) override;
	void Update(float fElapsedTime) override;
	void Render(float fElapsedTime) override;

	virtual void BuildScene();
	virtual void BuildRenderer();

private:
	std::unique_ptr<CGameScene>		m_pGameScene;
	std::unique_ptr<CRenderer>		m_pGameRenderer;

private:
	bool							m_bDrawWorldGizmo = false;
};

