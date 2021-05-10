#pragma once

#include <winsock2.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#include <WS2tcpip.h>


#include "d3dUtil.h"


using namespace DirectX;
using namespace DirectX::PackedVector;


#include "struct.h"
#include "define.h"
#include <map>

#include <iostream>

using namespace std;

struct Indices
{
	unsigned int i1, i2, i3;
};
struct MeshVtxInfo
{
	unsigned long		ulVertexCnt;
	vector<XMFLOAT3>	vecVtxPos;
	vector<Indices>		vecIdx;
};

#include "PhysXMgr.h"