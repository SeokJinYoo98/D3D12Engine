#pragma once
struct Polygons;

class CBaseMesh {
public:
	CBaseMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly, D3D12_PRIMITIVE_TOPOLOGY d3dTopo= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void Render(ID3D12GraphicsCommandList* pCommandList);
	void ReleaseUploadBuffers();

	void SetTopology(D3D12_PRIMITIVE_TOPOLOGY d3dTopo) { m_d3dPrimitiveTopology = d3dTopo; }

	
	struct SubMesh {
		UINT	IndexCount			= 0;
		UINT	StartIndexLocation	= 0;
		INT		BaseVertexLocation	= 0;

		// DirectX::BoundingBox Bounds;
	};
protected:
	void CreateMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly);
	virtual void CreateSubMeshes(Polygons* pNewPoly);
	virtual void CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly);
	virtual void CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList,  Polygons* pNewPoly);

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_pVertexUploadBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_pIndexUploadBuffer;

	std::unordered_map<std::string, CBaseMesh::SubMesh>	m_subMeshes;

	UINT			m_nVertexStride		= 0;
	UINT			m_nVertexBufferSize	= 0;
	DXGI_FORMAT		m_dxgiIndexFormat	= DXGI_FORMAT_R16_UINT;
	UINT			m_nIndex			= 0;
	UINT			m_nIndexBufferSize	= 0;
	UINT			m_nIndexStride		= 0;
	UINT			m_nSlot				= 0;

	D3D12_VERTEX_BUFFER_VIEW	m_d3dVertexBufferView	= { };
	D3D12_INDEX_BUFFER_VIEW		m_d3dIndexBufferView	= { };
	D3D12_PRIMITIVE_TOPOLOGY	m_d3dPrimitiveTopology;
};
