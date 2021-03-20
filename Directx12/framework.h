﻿// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


//#include <d3d12.h>
//#include <dxgi1_4.h>
//#include <D3Dcompiler.h>

//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif


#include <iostream>

#include <list>
#include <vector>
#include <map>

using namespace std;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

#include <wrl.h>
using namespace Microsoft::WRL;

#include "struct.h"
#include "define.h"

extern UINT gnCbvSrvUavDescriptorIncrementSize;
extern UINT gnRtvDescriptorIncrementSize;
extern UINT gnDsvDescriptorIncrementSize;

extern HINSTANCE   g_hInst;
extern HWND        g_hWnd;

#define WINCX 800
#define WINCY 600

enum OBJ_TYPE				{ OBJ_PLAYER, OBJ_CAMERA, OBJ_SKYBOX, OBJ_STATIC, OBJ_END };
enum RENDER_TYPE			{ RENDER_COLOR, RENDER_NOBLEND, RENDER_STATIC, RENDER_DYNAMIC, RENDER_SHADE, RENDER_SKYBOX, RENDER_END };
enum CAMERA_MODE			{ CAMERA_NONE, CAMERA_FREE, CAMERA_THIRD };
enum class MESH_TYPE		{ CHARACTER = 0, ROCK, HOUSE, TREE, TILE, COUNT };
enum class ANIMATION_TYPE	{ IDLE = 0, WALK, DANCE, NONE };
enum class PLAYER_STATE		{ IDLE = 0, WALK, NONE };
enum BoneIndex				{ WIZARD = 0, COUNT };


#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif