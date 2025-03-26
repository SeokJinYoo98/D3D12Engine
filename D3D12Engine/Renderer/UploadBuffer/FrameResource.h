#pragma once
#include "Renderer/UploadBuffer/UploadBuffer.h"


class CFrameResource
{
public:
    CFrameResource(ID3D12Device* pDevice, UINT nMesh, UINT nUI, UINT nPass);
    CFrameResource(const CFrameResource& rhs) = delete;
    CFrameResource& operator=(const CFrameResource& rhs) = delete;
    ~CFrameResource();

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      m_pCommandListAlloc     = nullptr;
    std::unique_ptr<CUploadBuffer<PassConstants>>       m_pPassCB               = nullptr;
    std::unique_ptr<CUploadBuffer<MeshConstants>>       m_pMeshCB               = nullptr;
    std::unique_ptr<CUploadBuffer<UIConstants>>   m_pDiffusedCB           = nullptr;

    UINT64 m_nFence = 0;
};

