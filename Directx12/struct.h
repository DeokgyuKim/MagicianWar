#pragma once


enum OBJ_TYPE					{ OBJ_PLAYER, OBJ_CAMERA, OBJ_SKYBOX, OBJ_STATIC, OBJ_BULLET, OBJ_SKILL, OBJ_UI, OBJ_END };
enum RENDER_TYPE				{ RENDER_COLOR, RENDER_NOBLEND, RENDER_STATIC, RENDER_DYNAMIC, RENDER_SHADE, RENDER_SKYBOX, RENDER_BULLET, RENDER_SKILL, RENDER_UI, RENDER_END };
enum CAMERA_MODE				{ CAMERA_NONE, CAMERA_FREE, CAMERA_THIRD };
enum class MESH_TYPE			{ CHARACTER = 0, ROCK, HOUSE, TREE, TILE, COUNT };
enum class ANIMATION_TYPE : int { IDLE = 0, WALK_FOWARD, WALK_BACK, WALK_LEFT, WALK_RIGHT, ATTACK, JUMP, NONE };
enum class PLAYER_STATE : int	{ IDLE = 0, MOVE, ATTACK, JUMP, NONE };
enum class BoneType : int		{ UPPER_BONE = 0, ROOT_BONE, NONE };
enum BoneIndex					{ WIZARD = 0, COUNT };
enum BUTTON_STATE				{ NONE, MOUSEON, ON };



struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;
};

struct ColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
};

struct SkyboxVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT3 Uv;
};


struct UvVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Uv;
	XMFLOAT3 Normal;
};

struct LightVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Uv;
};

struct SkinnedVertex// : Vertex
{ // Movable Model
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;
	XMFLOAT3 BoneWeights;
	BYTE BoneIndices[4];

	UINT MaterialIndex;
};

struct Material
{
	std::string Name;

	// Material constant buffer data used for shading.
	XMFLOAT3 Ambient = { 0.f,0.f,0.f };
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Specular = { 0.01f, 0.01f, 0.01f };

	int MaterialIndex = -1;
	
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct ObjectCB
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	UINT MaterialIndex; // 각자의 Object들에 해당하는 Index;
	UINT boolBone; 
};

struct SkillCB
{
	float fTime = 0.f;
	UINT  SkillIdx = 0;
	float DissolveC = 0.f;
	
};


struct MeshInfo
{
	int VtxCnt;
	int IdxCnt;
	//버텍스 정보, 인덱스 정보
};

struct CameraCB
{
	XMFLOAT4X4	View = MathHelper::Identity4x4();
	XMFLOAT4X4	Proj = MathHelper::Identity4x4();
	XMFLOAT4X4	InvView = MathHelper::Identity4x4();
	XMFLOAT4X4	InvProj = MathHelper::Identity4x4();
	XMFLOAT4	Position = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	float		Time = 0.f;
};

struct MaterialCB
{
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
	XMFLOAT4	Specular;
};

struct LightCB
{
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
	XMFLOAT4	Specular;
	XMFLOAT4	Position;
	XMFLOAT4	Direction;
};

struct SkinnedCB
{
	// 최대 뼈대 Character - 33 
	XMFLOAT3 right[33];
	XMFLOAT3 up[33];
	XMFLOAT3 look[33];
	XMFLOAT3 pos[33];

};

struct TransformStruct
{
	XMFLOAT3 xmfPosition;
	XMFLOAT3 xmfRotate;
	XMFLOAT3 xmfScale;
};

struct PlayerInfo
{
	DWORD		dwPlayerNum;
	DWORD		dwTeamNum;
	XMFLOAT3	xmfPosition;
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
};

struct STOC_OtherstartInfo {
	short size;
	unsigned char type;
	DWORD		dwPlayerNum;
	DWORD		dwTeamNum;
	XMFLOAT3	xmfPosition;
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
	XMFLOAT4X4 matWorld; // worldMatrix
	float lifeTime; // 생존시간
};

struct STOC_Bullet {
	short size;
	unsigned char type; 
	float Bullet_Count;
	Bullet_Packet bullets[100];
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