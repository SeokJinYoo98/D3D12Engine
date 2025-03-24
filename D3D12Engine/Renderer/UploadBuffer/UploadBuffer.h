#pragma once

struct MeshConstants {
    DirectX::XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
    DirectX::XMFLOAT4	xmf4Color = { 0.0f, 0.0f, 0.0f, 1.0f };
};
struct PassConstants {
    DirectX::XMFLOAT4X4 xmf4x4View = Matrix4x4::Identity();
    DirectX::XMFLOAT4X4 xmf4x4Projection = Matrix4x4::Identity();
};

template<typename T>
class CUploadBuffer
{
public:
    CUploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
        mIsConstantBuffer(isConstantBuffer)
    {
        mElementByteSize = sizeof(T);

        // Constant buffer elements need to be multiples of 256 bytes.
        // This is because the hardware can only view constant data 
        // at m*256 byte offsets and of n*256 byte lengths. 
        // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
        // UINT64 OffsetInBytes; // multiple of 256
        // UINT   SizeInBytes;   // multiple of 256
        // } D3D12_CONSTANT_BUFFER_VIEW_DESC;
        if (isConstantBuffer)
            mElementByteSize = ResourceHelper::CalcConstantBufferByteSize(sizeof(T));
        auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resureceDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);

        HRESULT hResult = device->CreateCommittedResource (
            &properties,
            D3D12_HEAP_FLAG_NONE,
            &resureceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mUploadBuffer)
        );
        if (FAILED(hResult)) 
            throw std::runtime_error("Failed to create UploadBuffer");

        hResult = mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData));
        if (FAILED(hResult))
            throw std::runtime_error("Failed to create UploadBuffer");
        // We do not need to unmap until we are done with the resource.  However, we must not write to
        // the resource while it is in use by the GPU (so we must use synchronization techniques).
    }

    CUploadBuffer(const CUploadBuffer& rhs) = delete;
    CUploadBuffer& operator=(const CUploadBuffer& rhs) = delete;
    ~CUploadBuffer()
    {
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    ID3D12Resource* Resource()const
    {
        return mUploadBuffer.Get();
    }

    void CopyData(int elementIndex, const T& data)
    {
        memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
    }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};
