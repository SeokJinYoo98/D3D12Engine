#pragma once
class CTexture
{
public:

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pUploadHeap = nullptr;
};

