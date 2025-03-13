#include "Common\pch.h"
#include "Mesh.h"

using namespace DirectX;

CMesh::CMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: m_d3dPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), m_d3dVertexBufferView{ }, m_d3dIndexBufferView{ },
	m_nSlot{ 0 }, m_nOffset { 0 }, m_nStride{ 0 }, m_nVertices{ 0 }, m_nIndices{ 0 }, m_nIndexOffset{ 0 }
{
}

CMesh::~CMesh()
{
}

void CMesh::ReleaseUploadBuffers()
{
	m_pVertexUploadBuffer.Reset();
	m_pIndexUploadBuffer.Reset();
}

void CMesh::Render(ID3D12GraphicsCommandList* pCommandList)
{
	// 메시의 프리미티브 유형을 설정
	pCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// 메시의 정점 버퍼 뷰를 설정
	pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	// 메시의 인덱스 버펴 뷰를 설정한다.
	pCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	// 메시의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력조립기)을 작동하게 한다.
	//pCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	// 인덱스를 사용해서 기본 도형을 그리려면 DrawIndexedInstanced를 사용
	pCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
}

CTriangleMesh::CTriangleMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	:CMesh(pDevice, pCommandList)
{
	CreateVertexBuffer(pDevice, pCommandList);
	CreateIndexBuffer(pDevice, pCommandList);
}

void CTriangleMesh::CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
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
		pDevice,
		pCommandList,
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

void CTriangleMesh::CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	std::vector<std::uint16_t> pIndices{
		0, 1, 2
	};

	m_nIndices = static_cast<UINT>(pIndices.size());
	m_nIndexStride = sizeof(std::uint16_t);
	UINT nIndexBufferSize = m_nIndexStride * m_nIndices;

	auto pRawBuffer = ResourceHelper::CreateBufferResource(
		pDevice,
		pCommandList,
		pIndices.data(),
		nIndexBufferSize,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pIndexUploadBuffer
	);
	m_pIndexBuffer.Attach(pRawBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes = nIndexBufferSize;
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

CCubeMesh::CCubeMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	: CMesh(pDevice, pCommandList)
{
	CreateVertexBuffer(pDevice, pCommandList);
	CreateIndexBuffer(pDevice, pCommandList);
}

void CCubeMesh::CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	auto& rd = Random::GetInstance();
	CDiffusedVertex pVertices[8] = {
		CDiffusedVertex(XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 0: 좌상앞 (빨강)
		CDiffusedVertex(XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 1: 우상앞 (초록)
		CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 2: 좌하앞 (파랑)
		CDiffusedVertex(XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 3: 우하앞 (노랑)
		CDiffusedVertex(XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 4: 좌상뒤 (자홍)
		CDiffusedVertex(XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 5: 우상뒤 (청록)
		CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 6: 좌하뒤 (흰색)
		CDiffusedVertex(XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT4(rd.GetRandomFloat(), rd.GetRandomFloat(), rd.GetRandomFloat(), 1.0f)), // 7: 우하뒤 (검정)
	};

	 auto pRawBuffer = ResourceHelper::CreateBufferResource(
		pDevice,
		pCommandList,
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pVertexUploadBuffer
	);
	 m_pVertexBuffer.Attach(pRawBuffer);

	 // 정점 버퍼 뷰를 생성
	 m_d3dVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	 m_d3dVertexBufferView.StrideInBytes = m_nStride;
	 m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

void CCubeMesh::CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	std::vector<std::uint16_t> pIndices {
		// Front
		0, 1, 2,	1, 3, 2,
		// Back
		5, 4, 7,	4, 6, 7,
		// Left
		4, 0, 6,	0, 2, 6,
		// Right
		1, 5, 3,	5, 7, 3,
		// Top
		4, 5, 0,	5, 1, 0,
		// Bottom
		2, 3, 6,	3, 7, 6
	};

	m_nIndices = static_cast<UINT>(pIndices.size());
	m_nIndexStride = sizeof(std::uint16_t);
	UINT nIndexBufferSize = m_nIndexStride * m_nIndices;

	auto pRawBuffer = ResourceHelper::CreateBufferResource(
		pDevice,
		pCommandList,
		pIndices.data(),
		nIndexBufferSize,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pIndexUploadBuffer
	);
	m_pIndexBuffer.Attach(pRawBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes = nIndexBufferSize;
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R16_UINT;
}
