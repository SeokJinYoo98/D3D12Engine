#pragma once
class CVertex {
public:
	CVertex() 
		: m_xmf3Position{ } { }
	CVertex(float x, float y, float z)
		: m_xmf3Position { x, y, z } {  }
	CVertex(const DirectX::XMFLOAT3& position)
		: m_xmf3Position{ position } { }

	~CVertex(){ }
protected:
	DirectX::XMFLOAT3 m_xmf3Position;
};

class CDiffusedVertex : public CVertex {
public:
	CDiffusedVertex() 
		: CVertex(), m_xmf4Diffuse{ } { }
	CDiffusedVertex(float x, float y, float z, float r, float g, float b, float a=1.0f)
		: CVertex(x, y, z), m_xmf4Diffuse{r, g, b, a} {  }
	CDiffusedVertex(float x, float y, float z, const DirectX::XMFLOAT4& diffuse)
		: CVertex(x, y, z), m_xmf4Diffuse{diffuse} {  }
	CDiffusedVertex(const DirectX::XMFLOAT3& position, float r, float g, float b, float a=1.0f)
		: CVertex(position), m_xmf4Diffuse{r, g, b, a} {  }
	CDiffusedVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& diffuse)
		: CVertex(position), m_xmf4Diffuse(diffuse){  }

	~CDiffusedVertex() { }

protected:
	// 정점의 색
	DirectX::XMFLOAT4 m_xmf4Diffuse;
};


class CMesh {
public:
	CMesh() { }
	CMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);

	virtual ~CMesh();
private:
	virtual void CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) { }
	virtual void CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) { }
	
public:
	void ReleaseUploadBuffers();
	virtual void ShowMyName() { std::cout << m_strName << std::endl; }
protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexUploadBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pIndexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	UINT m_nSlot;
	UINT m_nVertices;
	UINT m_nStride;
	UINT m_nOffset;

	UINT m_nIndices;
	UINT m_nIndexOffset;
	UINT m_nIndexStride{ };

	std::string m_strName{ "None" };
public:
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);
};

class CTriangleMesh : public CMesh {
public:
	CTriangleMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	~CTriangleMesh() { }

private:
	void CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) override;
	void CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) override;
};

class CCubeMesh :public CMesh {
public:
	CCubeMesh() { m_strName = "Cube"; }
	CCubeMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);

private:
	void CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) override;
	void CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) override;
};