#pragma once
using namespace DirectX;

class CVertex {
public:
	CVertex() 
		: m_xmf3Position{ } { }
	CVertex(float x, float y, float z)
		: m_xmf3Position { x, y, z } {  }
	CVertex(const XMFLOAT3& position)
		: m_xmf3Position{ position } { }

	~CVertex(){ }
protected:
	XMFLOAT3 m_xmf3Position;
};

class CDiffusedVertex : public CVertex {
public:
	CDiffusedVertex() 
		: CVertex(), m_xmf4Diffuse{ } { }
	CDiffusedVertex(float x, float y, float z, float r, float g, float b, float a=1.0f)
		: CVertex(x, y, z), m_xmf4Diffuse{r, g, b, a} {  }
	CDiffusedVertex(float x, float y, float z, const XMFLOAT4& diffuse)
		: CVertex(x, y, z), m_xmf4Diffuse{diffuse} {  }
	CDiffusedVertex(const XMFLOAT3& position, float r, float g, float b, float a=1.0f)
		: CVertex(position), m_xmf4Diffuse{r, g, b, a} {  }
	CDiffusedVertex(const XMFLOAT3& position, const XMFLOAT4& diffuse)
		: CVertex(position), m_xmf4Diffuse(diffuse){  }

	~CDiffusedVertex() { }

protected:
	// 정점의 색
	XMFLOAT4 m_xmf4Diffuse;
};
class CPolygon {
public:
	CPolygon() = default;
	CPolygon(int nIndices);

	void AddVertexIndex(int nIndex);
	void SetVertexVector(std::vector<int>&& nVertices);
public:
	std::vector<int> m_nVertexIndices;
};

class CMesh {
public:
	CMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, 
		const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);

	virtual ~CMesh();

public:
	void ReleaseUploadBuffers();

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

public:
	virtual void Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);
};

class CTriangleMesh : public CMesh {
public:
	CTriangleMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice,
		const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList);

	virtual ~CTriangleMesh() { }
};