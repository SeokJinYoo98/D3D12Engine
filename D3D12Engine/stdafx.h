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

#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl.h>



constexpr int FRAME_BUFFER_WIDTH = 800;
constexpr int FRAME_BUFFER_HEIGHT = 600;

extern bool CONSOLE_MESSAGE;
