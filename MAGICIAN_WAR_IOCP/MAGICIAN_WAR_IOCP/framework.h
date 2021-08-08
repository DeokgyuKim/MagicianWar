#pragma once

#include <mutex>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <algorithm>
#include <list>
#include <queue>
#include <map>
#include <vector>
#include <list>
#include <array>
#include <random>
#include <time.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <chrono>
#include <string>
#include <DirectXMath.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Ws2_32.lib")

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#include "d3dUtil.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;



#include "struct.h"
#include "PhysXMgr.h"