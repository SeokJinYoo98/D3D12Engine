#pragma once
class CMaterial {
public:
	CMaterial() = default;

public:
	DirectX::XMFLOAT4	xmf4DiffuseAlbedo;
	DirectX::XMFLOAT3	xmf3FresnelR0;
	float				fRoughness;
};