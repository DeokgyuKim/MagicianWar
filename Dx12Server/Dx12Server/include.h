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

struct PlayerInfo
{
	DWORD		dwPlayerNum;
	DWORD		dwTeamNum;
	XMFLOAT3	xmfPosition;
	int			iHp;
};

struct STOC_ServerPlayer
{
	short size;
	unsigned char type;
	PlayerInfo	info;
	SOCKET		socket;
};

struct CTOS_PlayerInfo
{
	short size;
	unsigned char type;
	unsigned char id; // 내 ID
	unsigned char InstanceName;
	XMFLOAT4X4		matWorld;
	PLAYER_STATE	ePlayerState;
	bool bAttackEnd;
};

struct STOC_PlayerInfo
{
	short size;
	unsigned char type;
	PlayerInfo		playerInfo;
	XMFLOAT4X4		matWorld;
	PLAYER_STATE	ePlayerState;

	ANIMATION_TYPE	Root_eAnimType;
	ANIMATION_TYPE	Upper_eAnimType;
	bool bAttackEnd;
};

struct Client_State { // 
	PlayerInfo		playerInfo;
	XMFLOAT4X4		matWorld;
	PLAYER_STATE	ePlayerState;
	ANIMATION_TYPE	eAnimType;
	float			fAnimTime;
	ANIMATION_TYPE	eAnimBlendType;
	float			fWeight;
	bool			isConnected;
};

struct CTOS_Ready {
	short size;
	unsigned char type;
	unsigned char id;
	DWORD ready;
};

struct CTOS_Skill {
	short size;
	unsigned char type;
	unsigned char id; // 누가 쏘는지
	unsigned char skill_type; // 스킬 타입
	XMFLOAT4X4		matWorld;
	// 또 뭐 필요하지
};

struct CTOS_keyInput {
	short size;
	unsigned char type;
	unsigned char id;
	DWORD key; // 키입력
};

struct STOC_sceneChange {
	short size;
	unsigned char type;
	DWORD sceneNum; // 0 : Logo, 1 : Lobby, 2 : mainScene
};

struct STOC_startInfo {
	short size;
	unsigned char type;
	DWORD		dwPlayerNum;
	DWORD		dwTeamNum;
	XMFLOAT3	xmfPosition;
	UINT		iHp;
};

struct STOC_OtherstartInfo {
	short size;
	unsigned char type;
	DWORD		dwPlayerNum;
	DWORD		dwTeamNum;
	XMFLOAT3	xmfPosition;
	UINT		iHp;
};

struct STOC_otherPlayerCount {
	short size;
	unsigned char type;
	DWORD playerCount;
};

struct CTOS_LoadingEnd {
	short size;
	unsigned char type;
	bool bLoadingEnd;
};

struct Bullet_Packet {
	unsigned char id; // 어떤 플레이어
	unsigned char InstanceName; // 어떤 캐릭터가
	float lifeTime; // 생존시간
	XMFLOAT4X4 matWorld; // worldMatrix
};

struct STOC_Bullet {
	short size;
	unsigned char type;
	int Bullet_Count;
	Bullet_Packet bullets[BulletCB_Count];
};

struct STOC_Skill {
	short size;
	unsigned char type;
	unsigned char id; // 어떤 플레이어
	unsigned char InstanceName; // 어떤 캐릭터가
	unsigned char skill_type; // 스킬 타입
	XMFLOAT4X4 matWorld; // worldMatrix
	float lifeTime; // 생존시간
	float shaderVariable1; // 셰이더 변수
	float shaderVariable2;
	float shaderVariable3;
	float shaderVariable4;
};

struct STOC_GameEnd {
	short size;
	unsigned char type;
	int teamNum;
	bool bEnd;
};