#pragma once
#include "Renderer/D3D12Device/D3D12Device.h"
#include "GameResource/ResourceManager.h"
#include "UploadBuffer/FrameResource.h"


class CGameScene;

class CRenderer : public CD3D12Device
{
public:
	CRenderer();
public:
	void	InitRenderer(HWND hWnd, UINT nWidth, UINT nHeight);

	void	Render(CGameScene* pGameScene, bool bDrawGrid);
	void	Update(CGameScene* pGameScene);
	void	BuildResourceManager();

	CResourceManager* GetResourceManager() { return m_pResourceManager.get(); }
private:
	void	UpdatePassCB(CGameScene* pGameScene);
	void	UpdateMeshCB(CGameScene* pGameScene);
	void	UpdateMaterialCB();

	void	RenderScene(CGameScene* pGameScene);
	void	DrawGrid(bool bDrawGrid);
private:
	
	void	BuildConstantBufferViews(UINT nMesh, UINT nUI, UINT nPass);
	void	BuildGrid();

private:
	std::unique_ptr<CResourceManager>					m_pResourceManager;
	std::unique_ptr<CUploadBuffer<MeshConstants>>		m_pMeshCBs;
	std::unique_ptr<CUploadBuffer<PassConstants>>		m_pPassCBs;
	std::unique_ptr<CUploadBuffer<MaterialConstants>>	m_pMaterialCBs;

	UINT m_nMeshCBByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(MeshConstants));
	UINT m_nMatCBByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	UINT					m_nMeshOffset	= 0;
	UINT					m_nPassOffset	= 0;
	PassConstants			m_mainPassCB	= { };
	
	UINT					m_nMaterialCBOffset = 0;
	std::array<MeshConstants, 3> m_gridConstants;
};

