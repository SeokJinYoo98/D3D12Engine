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
	// TODO:: ���� �߻�
	// ���� ���۸� ���� ���ε� ���۸� �Ҹ�
	if (m_pVertexUploadBuffer) {
		m_pVertexUploadBuffer.Reset();
		m_pVertexUploadBuffer = nullptr;
	}
		
}

void CMesh::Render(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
{
	// �޽��� ������Ƽ�� ������ ����
	pCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// �޽��� ���� ���� �並 ����
	pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	// �޽��� ���� ���� �並 �������Ѵ�(����������(�Է�������)�� �۵��ϰ� �Ѵ�.
	pCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

CTriangleMesh::CTriangleMesh(const Microsoft::WRL::ComPtr<ID3D12Device>& pDevice, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCommandList)
	:CMesh(pDevice, pCommandList)
{
	// �ﰢ�� �޽��� ����
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// ������ ������ �ð���� ������� �� �� �ķ������Ѵ�.
	// �� �Ķ���ʹ� 0.0 ~ 1.0 ������ �Ǽ� ��
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(Direction::UP, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(Direction::RIGTH, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(Direction::LEFT, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	// �ﰢ�� �޽��� ���ҽ��� ����
	m_pVertexBuffer = ResourceHelper::CreateBufferResource(
		pDevice.Get(),
		pCommandList.Get(),
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pVertexUploadBuffer
	);

	// ���� ���� �並 ����
	m_d3dVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
