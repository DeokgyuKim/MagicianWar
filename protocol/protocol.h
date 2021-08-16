#pragma once


#define SERVER_PORT 9000
#define SERVER_KEY	9999
#define EVENT_KEY	9990

#define BulletCB_Count 70



// Server ==> Client

// Common	1 ~ 30
#define stoc_SceneChange 1


// Loading	Scene	31 ~ 60
#define stoc_Accept_OK 31


#define stoc_otherPlayerNum 29



// Title	Scene	61 ~ 90
// Lobby	Scene	91 ~ 120
#define stoc_Room_Make_OK 91
#define stoc_Room_Make_Deny 92
#define stoc_Room_Join_OK 93
#define stoc_Room_Join_Deny 94
#define stoc_Room_Break_OK 95
#define stoc_RoomPlayer_Enter	96
#define stoc_RoomList_Update 97

// Room		Scene	121 ~ 150
#define stoc_Game_Start	121
#define stoc_RoomPlayer_Change	122
#define stoc_RoomPlayer_Leave	123

// Stage	Scene	151 ~ 180
#define stoc_InGame_StartInfo	151
#define stoc_playerInfo 152
#define stoc_bullet_update 153
#define stoc_bullet_delete 154
#define stoc_roundend 155
#define stoc_roundstart 156
#define stoc_left_shopping_time 157
#define stoc_roundreset 158
#define stoc_add_kill_point 159
#define stoc_Ingame_OutPlayer 160
#define ctos_skill_Request 161
#define stoc_skill			162
#define stoc_skillUpdate	163
#define stoc_skillDelete	164
// Result	Scene	181 ~ 210
// Ending	Scene	211 ~ 240

//// Client ==> Server

// Common			1 ~ 30

#define ctos_playerInfo		1
#define ctos_keyInput		2
#define ctos_Camera_update	3

// Loading	Scene	31 ~ 60
#define ctos_LoadingEnd 31
#define ctos_Connect_OK	32

// Title	Scene	61 ~ 90
// Lobby	Scene	91 ~ 120
#define ctos_Room_Make_Request 91
#define ctos_Room_Join_Request 92
#define ctos_Room_List_Request 93

// Room		Scene	121 ~ 150
#define ctos_RoomInfo_Request 121
#define ctos_Ready_OK	122
#define ctos_Ready_NONE 123
#define ctos_Team_Change 124
#define ctos_Character_Change 125
#define ctos_Game_Start 126
#define ctos_Room_Exit 127
// Stage	Scene	151 ~ 180
#define ctos_IngameInfo_Request 151
#define ctos_AttackEnd			152
#define ctos_ShoppingStart_Request 153
#define ctos_CreateBullet_Request 154
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
#define ctos_KEY_Q			0x0080 


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
#define STATE_FREEZE	7
#define STATE_ICE		8
#define STATE_COUNT		9 

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
#define ANIM_SKILL_ATTACK	10
#define ANIM_COUNT			11

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

// Skill Type
#define SKILL_Q				0
#define SKILL_E				1

#define SKILL_FIREWALL			2
#define SKILL_FIRE2				3

#define SKILL_COLD1				4
#define SKILL_COLD2				5

#define SKILL_DARKNESS1			6
#define SKILL_DARKNESS2			7
#define SKILL_CRESSVAS			8

struct PlayerInfo
{
	int					Client_Num;
	int					Room_Num;
	bool				isRoom_Host;
	char				TeamType;
	XMFLOAT3			xmfPosition;
	float				CameraY;
	unsigned char		CharacterType; // 캐릭터 타입
	int					PlayerState;   // 플레이어 현재 상태
	int					iHp;
	int					killPoint;
	bool				isreadyState;
};
struct Room_PlayerInfo
{
	bool isRoom_Host;
	char TeamNum;
	char CharacterType;
};
struct RoomInfo
{
	int					Room_Num;
	string				Room_Name;
	int					HostPlayer;
	int					TotalRound;
	int					curRound;
};

struct RoomPlayer
{
	bool used;
	bool ishost;
	bool readyState;
	char characterType;
	char slot_num;
	int	 id;
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

struct BulletInfo {
	int user; // 어떤플레이어
	unsigned char ElementType;	// Fire, Cold, Dark
	unsigned char Team; // 어떤 팀의
	int Damage;
	XMFLOAT4X4 matWorld;
};

struct Bullet_Packet {
	int id; // 어떤 플레이어
	unsigned char InstanceName; // 어떤 캐릭터가
	float lifeTime; // 생존시간
	XMFLOAT4X4 matWorld; // worldMatrix
};

//// Server ==> Client

// Lobby Scene
struct STOC_Accept_OK {
	short size;
	unsigned char type;
	int id;
};

struct STOC_ROOM_MAKE_OK {
	short size;
	unsigned char type;
	int Host_num;
	int room_num;
};

struct STOC_ROOM_MAKE_DENY {
	short size;
	unsigned char type;
};

struct STOC_ROOM_JOIN {
	short size;
	unsigned char type;
	int room_num;
};

struct STOC_ROOM_CHANGE {
	short size;
	unsigned char type;
	bool used;
	bool host;
	bool readyState;
	char characterType;
	char slot_num;
	int id;
};

struct STOC_ROOM_EXIT {

};

struct STOC_ROOM_ENTER {
	short size;
	unsigned char type;
	int room_num;
};

struct STOC_ROOM_LEAVE {
	short size;
	unsigned char type;
};

struct STOC_ROOM_BREAK {
	short size;
	unsigned char type;
	int room_num;
};
struct STOC_ROOMLIST_UPDATE {
	short size;
	unsigned char type;
	int room_num;
};

// Room Scene

struct STOC_GAME_START {
	short size;
	unsigned char type;
};
struct STOC_ROUND_START {
	short size;
	unsigned char type;
	unsigned char Cur_Round;
};
struct STOC_ROUND_RESET {
	short size;
	unsigned char type;
};

// InGame Scene

struct STOC_INGAME_STARTINFO {
	short size;
	unsigned char type;

	char				dwTeamNum;		// 내 팀 번호
	char				CharacterType;	// 캐릭터 타입
	XMFLOAT3			xmfPosition;	// 내 초기좌표
	int					iHp;			// 내 초기 HP
	int					id;				// ID
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
	char	ePlayerState;

	char	Root_eAnimType;
	char	Upper_eAnimType;
	bool bAttackEnd;
};


struct STOC_sceneChange {
	short size;
	unsigned char type;
	char sceneType; // 0 : Logo, 1 : Lobby, 2 : mainScene, 3 : GameEnd
};

struct STOC_Bullet_Update {
	short size;
	unsigned char type;
	unsigned char ElementType;
	unsigned char index;
	XMFLOAT4X4	  xmmWorld;
};

struct STOC_Bullet_Delete {
	short size;
	unsigned char type;
	unsigned char index;
};

struct STOC_SKILL_DELETE {
	short size;
	unsigned char type;
	unsigned char slotNum;
	unsigned char skillType;
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



struct STOC_Skill {
	short size;
	unsigned char type;
	int user;
	unsigned char slotNum;
	unsigned char skillType;
	XMFLOAT3 xmfPosition;
	XMFLOAT3 xmfRotate;
};

struct STOC_RoundEnd {
	short size;
	unsigned char type;
	unsigned char teamType;
};

struct STOC_LEFT_SHOPPING_TIME {
	short size;
	unsigned char type;
	short leftTime;
};

struct STOC_ADD_KILL_POINT {
	short size;
	unsigned char type;
};
struct STOC_INGAME_OUTPLAYER {
	short size;
	unsigned char type;
	int outPlayer_id;
};



//// Client ==> Server

// Loading Scene
struct CTOS_LoadingEnd {
	short size;
	unsigned char type;
	int id; // 어떤 플레이어
};

struct CTOS_Connect_OK {
	short size;
	unsigned char type;
};


struct CTOS_PlayerInfo
{
	short size;
	unsigned char type;
	int id; // 내 ID
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
	int room_num;			// 들어가고 싶은 방의 번호
};

// Room Scene

struct CTOS_ROOMINFO_REQUEST
{
	short size;
	unsigned char type;
	int room_num;
};

struct CTOS_ROOMLIST_REQUEST
{
	short size;
	unsigned char type;
};

struct CTOS_Ready {
	short size;
	unsigned char type;
};

struct CTOS_ROOM_EXIT {
	short size;
	unsigned char type;
};

struct CTOS_TEAMSELECT_REQUEST {
	short size;
	unsigned char type; 
	unsigned char teamType; // blue, red
};
struct CTOS_CHARACTER_CHANGE {
	short size;
	unsigned char type;
	unsigned char characterType; // fire, cold, carkness
};

struct CTOS_INGAMEINFO_REQUEST
{
	short size;
	unsigned char type;
	int room_num;
};

struct CTOS_Skill {
	short size;
	unsigned char type;
	XMFLOAT3	xmfPosition;
	XMFLOAT3	xmfRotate;
	unsigned char skill_type;
	// 또 뭐 필요하지
};

struct CTOS_KEYINPUT {
	short size;
	unsigned char type;
	DWORD key; // 키입력
};

struct CTOS_CAMERA {
	short size;
	unsigned char type;
	float CameraY;
	float CameraX;
};

struct CTOS_ATTACKEND {
	short size;
	unsigned char type;
	bool isAttacked;
};

struct CTOS_SHOPPINGSTART_REQUEST {
	short size;
	unsigned char type;
};

struct CTOS_CREATE_BULLET {
	short size;
	unsigned char type;
};