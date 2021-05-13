#pragma once
#include "framework.h"

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <winsock2.h>

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
	unsigned char id; // �� ID
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
	unsigned char id; // ���� �����
	unsigned char skill_type; // ��ų Ÿ��
	XMFLOAT4X4		matWorld;
	// �� �� �ʿ�����
};

struct CTOS_keyInput {
	short size;
	unsigned char type;
	unsigned char id;
	DWORD key; // Ű�Է�
};

struct STOC_sceneChange {
	short size;
	unsigned char type;
	DWORD sceneNum; // 0 : Logo, 1 : Lobby, 2 : mainScene, 3 : GameEnd
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
	unsigned char id; // � �÷��̾�
	unsigned char InstanceName; // � ĳ���Ͱ�
	float lifeTime; // �����ð�
	XMFLOAT4X4 matWorld; // worldMatrix
};

struct STOC_Bullet {
	short size;
	unsigned char type;
	int Bullet_Count;
	Bullet_Packet bullets[70];
};

struct Client_Bullet {
	unsigned char id; // � �÷��̾�
	unsigned char InstanceName; // � ĳ���Ͱ�
	float lifeTime; // �����ð�
	XMFLOAT4X4 matWorld; // worldMatrix
};

struct STOC_Skill {
	short size;
	unsigned char type;
	unsigned char id; // � �÷��̾�
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
	int teamNum;
	bool bEnd;
};
struct Client_GameEnd {
	int teamNum;
	bool bEnd;
};

class Player;
class Object;
class Network
{
private:
	Network() {}
	~Network() { Release(); }
private:
	static Network* m_pInstance;
public:
	static Network* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new Network;
		return m_pInstance;
	}
	static void			DestroyInstance()
	{
		if (m_pInstance)
			delete m_pInstance;
		m_pInstance = NULL;
	}
public:
	bool Init(const string& strServerIP = "127.0.0.1");
	void packetInit();
	bool Release();
	void Update();
	int  recvn(SOCKET s, char* buf, int len, int flags);
public:
	bool		GetLobbyEnd() { return m_bLobbyEnd; }
	XMFLOAT3	GetMyPlayerStartPos() { return m_tMyInfo.xmfPosition; }
	map<DWORD, PlayerInfo>	GetOtherPlayerInfo() { return m_mapOtherPlayerInfos; }
	STOC_PlayerInfo GetRecvPlayerInfo(DWORD playerNum);
	PlayerInfo	GetMyInfo() { return m_tMyInfo; }
	void		SetMyPlayerInfo(Player* pPlayer);
	void		SetOtherPlayerInfo(list<Object*>* plstPlayer);
	void		SetLoadingEnd() { LoadingEnd = true; }
	vector<Client_Bullet> GetBullets() { return m_vBullets; }
	Client_GameEnd GetGameEnd() { return m_CLgameEnd; }
	DWORD		GetCurScene() { return m_SceneChange; }
private:
	SOCKET	m_Sock;

private:
	//�ʱ⿡ �޾ƿ;� �� ����
	PlayerInfo	m_tMyInfo;
private:
	//�� �� �ܰ��� �����尡 ������ �� �Ǻ�
	bool	m_bLobbyEnd;
private:
	map<DWORD, PlayerInfo>		m_mapOtherPlayerInfos;
	map<DWORD, STOC_PlayerInfo> m_mapRecvPlayerInfos;
	vector<Client_Bullet>		m_vBullets;
	Client_GameEnd				m_CLgameEnd;
	int		m_iPlayerNum;
public:
	//Function For LobbyThread Send
	void SendReadyState();
	void SendMyPlayerInfo();
	void SendKeyInput();
	void SendLoadingEnd();
public:
	//Function For LobbyThread Recv
	bool IsMoveToMainGame();
	void RecvOtherPlayerInfo();


public:
	void recvUpdate();
	void recvProcessing(int _bytes);
	void packetProcessing(char* _packetBuffer);

private: // packets
	CTOS_keyInput KEY_packet;
	CTOS_PlayerInfo tInfo_packet;
	CTOS_Ready Ready_packet;
	CTOS_LoadingEnd LoadingEnd_packet;

	char* packet_start_ptr;
	char recvBuffer[MAX_BUFFER];
	char packetBuffer[MAX_BUFFER];
	short packet_size;
	int savedPacket_size;

	DWORD m_SceneChange;
	bool mainSceneLateInit;
	bool LoadingEnd;
};

