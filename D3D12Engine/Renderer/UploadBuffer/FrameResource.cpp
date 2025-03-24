#include "Common\pch.h"
#include "FrameResource.h"

CFrameResource::CFrameResource(ID3D12Device* pDevice, UINT nMesh, UINT nPass)
{
    pDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(m_pCommandListAlloc.GetAddressOf())
    );

    m_pPassCB   = std::make_unique<CUploadBuffer<PassConstants>>(pDevice, nPass, true);
    m_pMeshCB   = std::make_unique<CUploadBuffer<MeshConstants>>(pDevice, nMesh, true);
}

CFrameResource::~CFrameResource()
{
}
