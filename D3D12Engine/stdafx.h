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

#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl.h>

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

constexpr int FRAME_BUFFER_WIDTH = 800;
constexpr int FRAME_BUFFER_HEIGHT = 600;

extern bool CONSOLE_MESSAGE;
