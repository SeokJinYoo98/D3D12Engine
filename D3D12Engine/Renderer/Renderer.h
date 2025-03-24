#pragma once
#include "Renderer/D3D12Device/D3D12Device.h"
#include "Renderer/ResourceManager.h"
#include "UploadBuffer/FrameResource.h"

class CGameScene;
class CCamera;

class CRenderer : public CD3D12Device
{
public:
	CRenderer();
public:
	void	InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight);

	void	Render(CGameScene* pGameScene);
	void	Update(CGameScene* pGameScene);


	void	BeginRendering(const DirectX::XMFLOAT4& xmf4BgColor);
	void	EndRendering();
private:
	void	UpdatePassCB(CGameScene* pGameScene);
	void	UpdateMeshCB(CGameScene* pGameScene);

private:
	void	BuildResourceManager();
	void	BuildFrameResource(UINT nMesh, UINT nPass, UINT nFrame);
	void	BuildConstantBufferViews(UINT nMesh, UINT nPass, UINT nFrame);
	
	void	SetViewportsAndScissorRects(CCamera* pCamera);
	void	RenderScene(CGameScene* pGameScene);
private:
	std::unique_ptr<CUploadBuffer<MeshConstants>> m_pMeshCBs;
	std::unique_ptr<CUploadBuffer<PassConstants>> m_pPassCBs;

	std::vector<std::unique_ptr<CFrameResource>> m_pFrameResources;
	CFrameResource* m_pCurrFrameResource = nullptr;
	UINT m_nCurrFrameResourceIndex = -1;
	
	UINT m_nPassOffset = 0;
	UINT m_nFrameResource = 3;
	PassConstants m_mainPassCB;
};

