// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <Windows.h>
#include <windowsx.h>
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
#include <map>
#include <set>
#include <unordered_map>

#include <random>

#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include "d3dx12.h"

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <wrl.h>

constexpr int FRAME_BUFFER_WIDTH = 800;
constexpr int FRAME_BUFFER_HEIGHT = 600;

extern bool CONSOLE_MESSAGE;

constexpr int MAX_LIGHTS = 16;

namespace Random {
	class Generator {
	public:
		Generator(): m_gen(std::random_device{}()) { }

		int GetRandomInt(int minValue = 0, int maxValue = 100) {
			std::uniform_int_distribution<int> dist(minValue, maxValue);
			return dist(m_gen);
		}
		float GetRandomFloat(float minValue = 0.0f, float maxValue = 1.0f) {
			std::uniform_real_distribution<float> dist(minValue, maxValue);
			return dist(m_gen);
		}
		double GetRandomDouble(double minValue = 0.0, double maxValue = 1.0) {
			std::uniform_real_distribution<double> dist(minValue, maxValue);
			return dist(m_gen);
		}
	private:
		std::mt19937 m_gen;
	};
	inline Generator& GetInstance() {
		static Generator instance;
		return instance;
	}
}


namespace Direction {
	constexpr DirectX::XMFLOAT3 ORIGIN_POINT{ 0.0f, 0.0f, 0.0f };
	constexpr DirectX::XMFLOAT3 AXIS_X	{ +1.0f, +0.0f, +0.0f };
	constexpr DirectX::XMFLOAT3 AXIS_Y	{ +0.0f, +1.0f, +0.0f };
	constexpr DirectX::XMFLOAT3 AXIS_Z	{ +0.0f, +0.0f, +1.0f };

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
	UINT CalcConstantBufferByteSize(UINT byteSize);
}

namespace Vector3 {

	inline DirectX::XMFLOAT3 XMVectorToFloat3(const DirectX::XMVECTOR& xmvVector)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, xmvVector);
		return xmf3Result;
	}
	inline DirectX::XMFLOAT3 RotateY(const DirectX::XMFLOAT3& xmfvector, float angle)
	{
		DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&xmfvector);
		DirectX::XMMATRIX matRotation = DirectX::XMMatrixRotationY(angle);
		DirectX::XMVECTOR rotatedVec = DirectX::XMVector3Transform(vec, matRotation);
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, rotatedVec);
		return result;
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
		return xmf3Result;
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
	inline DirectX::XMFLOAT4 StoreFloat4(const DirectX::XMVECTOR& xmv4Vector)
	{
		DirectX::XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, xmv4Vector);
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
	inline DirectX::XMFLOAT4X4 TransposeAndPrint(const DirectX::XMFLOAT4X4& xmmtx4x4Matrix, const std::string& name)
	{
		// 행렬 출력 (트랜스포즈 전)
		std::cout << name << " before transpose:" << std::endl;
		std::cout << xmmtx4x4Matrix._11 << " " << xmmtx4x4Matrix._12 << " " << xmmtx4x4Matrix._13 << " " << xmmtx4x4Matrix._14 << std::endl;
		std::cout << xmmtx4x4Matrix._21 << " " << xmmtx4x4Matrix._22 << " " << xmmtx4x4Matrix._23 << " " << xmmtx4x4Matrix._24 << std::endl;
		std::cout << xmmtx4x4Matrix._31 << " " << xmmtx4x4Matrix._32 << " " << xmmtx4x4Matrix._33 << " " << xmmtx4x4Matrix._34 << std::endl;
		std::cout << xmmtx4x4Matrix._41 << " " << xmmtx4x4Matrix._42 << " " << xmmtx4x4Matrix._43 << " " << xmmtx4x4Matrix._44 << std::endl;

		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return xmmtx4x4Result;
	}
	inline void PrintMatrix4x4(const DirectX::XMFLOAT4X4& xmmtx4x4Matrix, const std::string& name)
	{
		std::cout << name << std::endl;
		std::cout << xmmtx4x4Matrix._11 << ", " << xmmtx4x4Matrix._12 << ", " << xmmtx4x4Matrix._13 << ", " << xmmtx4x4Matrix._14 << std::endl;
		std::cout << xmmtx4x4Matrix._21 << ", " << xmmtx4x4Matrix._22 << ", " << xmmtx4x4Matrix._23 << ", " << xmmtx4x4Matrix._24 << std::endl;
		std::cout << xmmtx4x4Matrix._31 << ", " << xmmtx4x4Matrix._32 << ", " << xmmtx4x4Matrix._33 << ", " << xmmtx4x4Matrix._34 << std::endl;
		std::cout << xmmtx4x4Matrix._41 << ", " << xmmtx4x4Matrix._42 << ", " << xmmtx4x4Matrix._43 << ", " << xmmtx4x4Matrix._44 << std::endl;
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
	inline DirectX::XMFLOAT3 GetPosition(const DirectX::XMFLOAT4X4& xmf4x4transform)
	{
		return DirectX::XMFLOAT3(xmf4x4transform._41, xmf4x4transform._42, xmf4x4transform._43);
	}
	inline DirectX::XMFLOAT3 GetForwardVector(const DirectX::XMFLOAT4X4& xmf4x4transform, bool bNormalize = true)
	{
		DirectX::XMFLOAT3 xmf3Forward(xmf4x4transform._31, xmf4x4transform._32, xmf4x4transform._33);
		if (bNormalize)
			xmf3Forward = Vector3::Normalize(xmf3Forward);

		return xmf3Forward;
	}
	inline DirectX::XMFLOAT3 GetRightVector(const DirectX::XMFLOAT4X4& xmf4x4transform, bool bNormalize = true)
	{
		DirectX::XMFLOAT3 xmf3Right(xmf4x4transform._11, xmf4x4transform._21, xmf4x4transform._31);
		if (bNormalize)
			xmf3Right = Vector3::Normalize(xmf3Right);

		return xmf3Right;
	}
}

