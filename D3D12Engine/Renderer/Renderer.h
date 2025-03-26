#pragma once
#include "Renderer/D3D12Device/D3D12Device.h"
#include "Renderer/ResourceManager.h"
#include "UploadBuffer/FrameResource.h"


class CGameScene;

class CRenderer : public CD3D12Device
{
public:
	CRenderer();
public:
	void	InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight);

	void	Render(CGameScene* pGameScene, bool bDrawGizmo);
	void	Update(CGameScene* pGameScene);

private:
	void	UpdatePassCB(CGameScene* pGameScene);
	void	UpdateMeshCB(CGameScene* pGameScene);
	void	RenderScene(CGameScene* pGameScene);

private:
	void	BuildResourceManager();
	void	BuildConstantBufferViews(UINT nMesh, UINT nUI, UINT nPass);

private:
	std::unique_ptr<CResourceManager>				m_pResourceManager;
	std::unique_ptr<CUploadBuffer<MeshConstants>>	m_pMeshCBs;
	std::unique_ptr<CUploadBuffer<PassConstants>>	m_pPassCBs;
	std::unique_ptr<CUploadBuffer<UIConstants>>		m_pUICBs;

	UINT			m_nPassOffset	= 0;
	UINT			m_nUIOffset		= 0;
	PassConstants	m_mainPassCB	= { };

};

