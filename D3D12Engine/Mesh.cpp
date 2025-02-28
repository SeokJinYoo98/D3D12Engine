#include "stdafx.h"
#include "Mesh.h"


CPolygon::CPolygon(int nIndices)
{
	m_nVertexIndices.reserve(nIndices);
}

void CPolygon::AddVertexIndex(int nIndex)
{	
	m_nVertexIndices.emplace_back(nIndex);
}

void CPolygon::SetVertexVector(std::vector<int>&& nVertices)
{
	m_nVertexIndices = std::move(nVertices);
}

CMesh::CMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
	: m_d3dPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

CMesh::~CMesh()
{
}

void CMesh::ReleaseUploadBuffers()
{
	// TODO:: 에러 발생
	// 정점 버퍼를 위한 업로드 버퍼를 소멸
	if (m_pVertexUploadBuffer) {
		m_pVertexUploadBuffer.Reset();
		m_pVertexUploadBuffer = nullptr;
	}
		
}

void CMesh::Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	// 메시의 프리미티브 유형을 설정
	pCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// 메시의 정점 버퍼 뷰를 설정
	pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	// 메시의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력조립기)을 작동하게 한다.
	pCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

CTriangleMesh::CTriangleMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
	:CMesh(pDevice, pCommandList)
{
	// 삼각형 메쉬를 설정
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 정점의 색상은 시계방향 순서대로 빨 녹 파로지정한다.
	// 각 파라미터는 0.0 ~ 1.0 사이의 실수 값
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(Direction::UP, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(Direction::RIGTH, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(Direction::LEFT, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	// 삼각형 메쉬를 리소스로 생성
	m_pVertexBuffer = ResourceHelper::CreateBufferResource(
		pDevice.Get(),
		pCommandList.Get(),
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pVertexUploadBuffer
	);

	// 정점 버퍼 뷰를 생성
	m_d3dVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
