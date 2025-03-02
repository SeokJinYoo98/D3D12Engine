// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// C 런타임 헤더 파일입니다.
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <memory>
#include <vector>
#include <array>
#include <random>

#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <wrl.h>

constexpr int FRAME_BUFFER_WIDTH = 800;
constexpr int FRAME_BUFFER_HEIGHT = 600;

extern bool CONSOLE_MESSAGE;

namespace Random {
	std::random_device rd{ };
	std::mt19937 gen(rd());
	std::uniform_int_distribution uid{ };
	std::uniform_real_distribution<float> ufd{ 0.0f, 1.0f };
	std::uniform_real_distribution<double> udd{ 0.0, 1.0 };

	int GetRandomInt(int minValue=0, int maxValue=100);
	float GetRandomFloat(float minValue = 0.0f, float maxValue = 1.0f);
	double GetRandomDouble(double minValue = 0.0, double maxValue = 1.0);

}


namespace Direction {
	constexpr DirectX::XMFLOAT3 ORIGIN_POINT{ 0.0f, 0.0f, 0.0f };

	constexpr DirectX::XMFLOAT3 UP		{ +0.0f, +1.0f, +0.0f };
	constexpr DirectX::XMFLOAT3 DOWN	{ +0.0f, -1.0f, +0.0f };
	constexpr DirectX::XMFLOAT3 LEFT	{ -1.0f, +0.0f, +0.0f };
	constexpr DirectX::XMFLOAT3 RIGTH	{ +1.0f, +0.0f, +0.0f };
	constexpr DirectX::XMFLOAT3 FORWARD	{ +0.0f, +0.0f, +1.0f };
	constexpr DirectX::XMFLOAT3 BACKWARD{ +0.0f, +0.0f, -1.0f };
}
namespace ResourceHelper {
	ID3D12Resource* CreateBufferResource(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		void* pData, UINT nBytes,
		D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		ID3D12Resource** ppd3dUploadBuffer = nullptr
	);
}

namespace Vector3 {
	inline DirectX::XMFLOAT3 XMVectorToFloat3(const DirectX::XMVECTOR& xmvVector)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, xmvVector);
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 ScalarProduct(const DirectX::XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMVECTOR xmv3Temp = DirectX::XMLoadFloat3(&xmf3Vector);
		if (bNormalize) xmv3Temp = DirectX::XMVector3Normalize(xmv3Temp);
		DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVectorScale(xmv3Temp, fScalar));
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVectorAdd(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2, float fScalar)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMVECTOR sum = DirectX::XMVectorAdd(DirectX::XMVectorScale(XMLoadFloat3(&xmf3Vector1), fScalar), XMLoadFloat3(&xmf3Vector2));
		XMStoreFloat3(&xmf3Result, sum);
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 Subtract(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, DirectX::XMVectorSubtract(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return xmf3Result;
	}
	inline float DotProduct(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, DirectX::XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return xmf3Result.x;
	}
	inline DirectX::XMFLOAT3 CrossProduct(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2, bool bNormalize=true)
	{
		DirectX::XMFLOAT3 xmf3Result;
		auto cross = DirectX::XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2));
		if (bNormalize)
			cross = DirectX::XMVector3Normalize(cross);
		XMStoreFloat3(&xmf3Result, cross);
	}
	inline DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& xmf3Vector)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return xmf3Result;
	}
	inline float Length(const DirectX::XMFLOAT3& xmf3Vector)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return xmf3Result.x;
	}
	inline float Angle(const DirectX::XMVECTOR& xmvVector1, const DirectX::XMVECTOR& xmvVector2)
	{
		DirectX::XMVECTOR xmvAngle = DirectX::XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return DirectX::XMConvertToDegrees(acosf(DirectX::XMVectorGetX(xmvAngle)));
	}
	inline float Angle(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2)
	{
		return Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2));
	}
	inline DirectX::XMFLOAT3 TransformNormal(const DirectX::XMFLOAT3& xmf3Vector, const DirectX::XMMATRIX& xmmtxTransform)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, DirectX::XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 TransformCoord(const DirectX::XMFLOAT3& xmf3Vector, const DirectX::XMMATRIX& xmmtxTransform)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 TransformCoord(const DirectX::XMFLOAT3& xmf3Vector, const DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix));
	}
}

namespace Vector4 {
	inline DirectX::XMFLOAT4 Add(const DirectX::XMFLOAT4& xmf4Vector1, const DirectX::XMFLOAT4& xmf4Vector2)
	{
		DirectX::XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, DirectX::XMVectorAdd(XMLoadFloat4(&xmf4Vector1), XMLoadFloat4(&xmf4Vector2)));
		return xmf4Result;
	}
	inline DirectX::XMFLOAT4 Multiply(const DirectX::XMFLOAT4& xmf4Vector1, const DirectX::XMFLOAT4& xmf4Vector2)
	{
		DirectX::XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, DirectX::XMVectorMultiply(XMLoadFloat4(&xmf4Vector1), XMLoadFloat4(&xmf4Vector2)));
		return xmf4Result;
	}
	inline DirectX::XMFLOAT4 Multiply(const DirectX::XMFLOAT4& xmf4Vector1, float fScalar)
	{
		DirectX::XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, DirectX::XMVectorScale(XMLoadFloat4(&xmf4Vector1), fScalar));
		return xmf4Result;
	}
}


namespace Matrix4x4
{
	inline DirectX::XMFLOAT4X4 Identity()
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixIdentity());
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 Multiply(const DirectX::XMFLOAT4X4& xmmtx4x4Matrix1, const DirectX::XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 Multiply(const DirectX::XMFLOAT4X4& xmmtx4x4Matrix1, const DirectX::XMMATRIX& xmmtxMatrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 Multiply(const DirectX::XMMATRIX& xmmtxMatrix1, const DirectX::XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 Inverse(const DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 Transpose(const DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return xmmtx4x4Result;
	}
	inline DirectX::XMFLOAT4X4 LookAtLH(const DirectX::XMFLOAT3& xmf3EyePosition, const DirectX::XMFLOAT3& xmf3LookAtPosition, const DirectX::XMFLOAT3& xmf3UpDirection)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixLookAtLH(
			XMLoadFloat3(&xmf3EyePosition),
			XMLoadFloat3(&xmf3LookAtPosition), 
			XMLoadFloat3(&xmf3UpDirection)));
		return xmmtx4x4Result;
	}
}

