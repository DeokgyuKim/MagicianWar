#pragma once


#define SERVER_PORT 9000
#define SERVER_KEY	9999

#define BulletCB_Count 70

// Character Default MoveSpeed
#define P_MoveForward_Speed 0.05f
#define P_MoveLeft_Speed 0.05f
#define P_MoveRight_Speed 0.05f
#define P_MoveBackward_Speed 0.03f

// Server ==> Client

// Common	1 ~ 30
#define stoc_sceneChange 1


// Loading	Scene	31 ~ 60
#define stoc_Accept_OK 31

#define stoc_otherPlayerNum 29
#define stoc_OtherstartInfo 30
#define stoc_playerInfo 33
#define stoc_SkillPacket 34
#define stoc_bullet 35
#define stoc_gameend 36

#define packet_skill 21
#define packet_keyInput 22

// Title	Scene	61 ~ 90
// Lobby	Scene	91 ~ 120
#define stoc_Room_Make_OK 91
#define stoc_Room_Make_Deny 92
#define stoc_Room_Join_OK 91
#define stoc_Room_Join_Deny 92

// Room		Scene	121 ~ 150
#define stoc_Game_Start	121
// Stage	Scene	151 ~ 180
// Result	Scene	181 ~ 210
// Ending	Scene	211 ~ 240

//// Client ==> Server

// Common			1 ~ 30

#define ctos_playerInfo 1

// Loading	Scene	31 ~ 60
#define ctos_LoadingEnd 31
#define ctos_Connect_OK	32

// Title	Scene	61 ~ 90
// Lobby	Scene	91 ~ 120
#define ctos_Room_Make_Request 91
#define ctos_Room_Join_Request 92

// Room		Scene	121 ~ 150
#define ctos_Ready_OK	121
#define ctos_Ready_NONE 122
#define ctos_BlueTeam_Request 123
#define ctos_RedTeam_Request 124
#define ctos_Room_Exit 124
// Stage	Scene	151 ~ 180
// Result	Scene	181 ~ 210
// Ending	Scene	211 ~ 240

// KeyInput
#define ctos_KEY_W			0x0001 
#define ctos_KEY_A			0x0002 
#define ctos_KEY_S			0x0004 
#define ctos_KEY_D			0x0008 
#define ctos_KEY_SPACE		0x0010 
#define ctos_KEY_LBUTTON	0x0020 
#define ctos_KEY_E			0x0040 

// Scene
#define LOADING_SCENE	0
#define TITLE_SCENE		1
#define LOBBY_SCENE		2
#define ROOM_SCENE		3
#define STAGE_SCENE		4
#define RESULT_SCENE	5
#define ENDING_SCENE	6

// Player State
#define STATE_IDLE		0
#define STATE_MOVE		1
#define STATE_ATTACK	2
#define STATE_JUMP		3
#define STATE_HIT		4
#define STATE_DEAD		5
#define STATE_DANCE		6
#define STATE_COUNT		7 

// Anim
#define ANIM_IDLE			0
#define ANIM_WALK_FOWARD	1
#define ANIM_WALK_BACK		2
#define ANIM_WALK_LEFT		3
#define ANIM_WALK_RIGHT		4
#define ANIM_ATTACK			5
#define ANIM_JUMP			6
#define ANIM_HIT			7
#define ANIM_DEAD			8
#define ANIM_DANCE			9
#define ANIM_COUNT			10

// Bone Type
#define BONE_UPPER			0
#define BONE_ROOT			1
#define BONE_COUNT			2

// Team
#define TEAM_NONE			0
#define TEAM_BLUE			1
#define TEAM_RED			2

// Character Type
#define ELEMENT_FIRE		1
#define ELEMENT_COLD		2
#define ELEMENT_DARKNESS	3

struct PlayerInfo
{
	int					Client_Num;
	int					Room_Num;
	char				dwTeamNum;
	XMFLOAT4X4			matWorld;
	XMFLOAT3			xmfPosition;
	unsigned char		CharacterType; // ĳ���� Ÿ��
	int					PlayerState;   // �÷��̾� ���� ����
	int					iHp;
};
struct RoomInfo
{
	int					Room_Num;
	string				Room_Name;
};

struct Client_State { // 
	PlayerInfo		playerInfo;
	XMFLOAT4X4		matWorld;
	int				ePlayerState;
	int				eAnimType;
	float			fAnimTime;
	int				eAnimBlendType;
	float			fWeight;
	bool			isConnected;
};

struct Bullet_Packet {
	int id; // � �÷��̾�
	unsigned char InstanceName; // � ĳ���Ͱ�
	float lifeTime; // �����ð�
	XMFLOAT4X4 matWorld; // worldMatrix
};

//// Server ==> Client

// Lobby Scene
struct STOC_Accept_OK {
	short size;
	unsigned char type;
	int id;
};

struct STOC_ROOM_MAKE {
	short size;
	unsigned char type;
	
};

struct STOC_ROOM_JOIN {
	short size;
	unsigned char type;
	
};

// Room Scene

struct STOC_GAME_START {
	short size;
	unsigned char type;

	char				dwTeamNum;		// �� �� ��ȣ
	XMFLOAT3			xmfPosition;	// �� �ʱ���ǥ
	char				CharacterType;	// ĳ���� Ÿ��
	int					iHp;			// �� �ʱ� HP
	
};

struct STOC_ServerPlayer
{
	short size;
	unsigned char type;
	PlayerInfo	info;
	SOCKET		socket;
};

struct STOC_PlayerInfo
{
	short size;
	unsigned char type;
	PlayerInfo		playerInfo;
	XMFLOAT4X4		matWorld;
	int	ePlayerState;

	int	Root_eAnimType;
	int	Upper_eAnimType;
	bool bAttackEnd;
};


struct STOC_sceneChange {
	short size;
	unsigned char type;
	DWORD sceneNum; // 0 : Logo, 1 : Lobby, 2 : mainScene, 3 : GameEnd
};


struct STOC_OtherstartInfo {
	short size;
	unsigned char type;
	unsigned char CharacterType;
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

struct STOC_Bullet {
	short size;
	unsigned char type;
	int Bullet_Count;
	Bullet_Packet bullets[BulletCB_Count];
};

struct STOC_Skill {
	short size;
	unsigned char type;
	int id; // � �÷��̾�
	unsigned char InstanceName; // � ĳ���Ͱ�
	unsigned char skill_type; // ��ų Ÿ��
	XMFLOAT4X4 matWorld; // worldMatrix
	float lifeTime; // �����ð�
	float shaderVariable1; // ���̴� ����
	float shaderVariable2;
	float shaderVariable3;
	float shaderVariable4;
};

struct STOC_GameEnd {
	short size;
	unsigned char type;
	unsigned char teamNum;
	bool bEnd;
};

//// Client ==> Server

// Loading Scene
struct CTOS_LoadingEnd {
	short size;
	unsigned char type;
	int id; // � �÷��̾�
};

struct CTOS_Connect_OK {
	short size;
	unsigned char type;
};


struct CTOS_PlayerInfo
{
	short size;
	unsigned char type;
	int id; // �� ID
	unsigned char InstanceName;
	XMFLOAT4X4		matWorld;
	int	ePlayerState;
	bool bAttackEnd;
};

// Lobby Scene
struct CTOS_ROOM_MAKE_REQUEST
{
	short size;
	unsigned char type;
};
struct CTOS_ROOM_JOIN_REQUEST
{
	short size;
	unsigned char type;
	int room_num;			// ���� ���� ���� ��ȣ
};

// Room Scene

struct CTOS_Ready {
	short size;
	unsigned char type;
};

struct CTOS_ROOM_EXIT {
	short size;
	unsigned char type;
};


struct CTOS_Skill {
	short size;
	unsigned char type;
	int id; // ���� �����
	unsigned char skill_type; // ��ų Ÿ��
	XMFLOAT4X4		matWorld;
	// �� �� �ʿ�����
};

struct CTOS_keyInput {
	short size;
	unsigned char type;
	int id;
	DWORD key; // Ű�Է�
};

