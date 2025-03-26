#include "Common\pch.h"
#include "Mesh.h"
#include "Polygon/Polygon.h"

CBaseMesh::CBaseMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly, D3D12_PRIMITIVE_TOPOLOGY d3dTopo)
	:m_d3dPrimitiveTopology{ d3dTopo }
{
	CreateMesh(pDevice, pCommandList, pNewPoly);
}

void CBaseMesh::Render(ID3D12GraphicsCommandList* pCommandList)
{
	// 메시의 프리미티브 유형을 설정
	pCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// 메시의 정점 버퍼 뷰를 설정
	pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	// 메시의 인덱스 버펴 뷰를 설정한다.
	pCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);

	pCommandList->DrawIndexedInstanced(m_nIndex, 1, 0, 0, 0);
}

void CBaseMesh::ReleaseUploadBuffers()
{
	m_pVertexUploadBuffer.Reset();
	m_pIndexUploadBuffer.Reset();
}


void CBaseMesh::CreateMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly)
{
	CreateSubMeshes(pNewPoly);
	CreateVertexBuffer(pDevice, pCommandList, pNewPoly);
	CreateIndexBuffer(pDevice, pCommandList, pNewPoly);
}

void CBaseMesh::CreateSubMeshes(Polygons* pNewPoly)
{
	for (const auto& subPoly : pNewPoly->SubPolygons) {
		CBaseMesh::SubMesh subMesh;
		subMesh.BaseVertexLocation = subPoly.BaseVertexLocation;
		subMesh.IndexCount = subPoly.IndexCount;
		subMesh.StartIndexLocation = subPoly.StartIndexLocation;

		m_subMeshes[subPoly.name] = subMesh;
	}
}

void CBaseMesh::CreateVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly)
{
	m_nVertexStride			= pNewPoly->VertexStride;
	m_nVertexBufferSize		= pNewPoly->VertexCount * pNewPoly->VertexStride;

	auto pRawPtr = ResourceHelper::CreateBufferResource(
		pDevice,
		pCommandList,
		pNewPoly->Vertices.data(),
		m_nVertexBufferSize,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pVertexUploadBuffer
	);

	m_pVertexBuffer.Attach(pRawPtr);
	m_d3dVertexBufferView.BufferLocation	= m_pVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes		= m_nVertexStride;
	m_d3dVertexBufferView.SizeInBytes		= m_nVertexBufferSize;
}

void CBaseMesh::CreateIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, Polygons* pNewPoly)
{
	m_nIndexStride		= (pNewPoly->IndexFormat == DXGI_FORMAT_R16_UINT) ? sizeof(uint16_t) : sizeof(uint32_t);
	m_nIndex			= pNewPoly->IndexCount;
	m_nIndexBufferSize	= m_nIndexStride * pNewPoly->IndexCount;

	if (pNewPoly->IndexFormat == DXGI_FORMAT_R16_UINT) {
		auto pRawBuffer = ResourceHelper::CreateBufferResource(
			pDevice,
			pCommandList,
			pNewPoly->Indices16.data(),
			m_nIndexBufferSize,
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_RESOURCE_STATE_INDEX_BUFFER,
			&m_pIndexUploadBuffer
		);
		m_pIndexBuffer.Attach(pRawBuffer);
	}
	else {
		auto pRawBuffer = ResourceHelper::CreateBufferResource(
			pDevice,
			pCommandList,
			pNewPoly->Indices32.data(),
			m_nIndexBufferSize,
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_RESOURCE_STATE_INDEX_BUFFER,
			&m_pIndexUploadBuffer
		);
		m_pIndexBuffer.Attach(pRawBuffer);
	}

	m_d3dIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes	= m_nIndexBufferSize;
	m_d3dIndexBufferView.Format			= pNewPoly->IndexFormat;
}
