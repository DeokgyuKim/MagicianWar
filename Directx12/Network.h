#pragma once
#include "framework.h"

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <winsock2.h>

#include <cstdarg>
#include "protocol.h"

struct Client_Bullet {
	unsigned char id; // 어떤 플레이어
	unsigned char InstanceName; // 어떤 캐릭터가
	float lifeTime; // 생존시간
	XMFLOAT4X4 matWorld; // worldMatrix
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
	bool Release();
	void Update();
	int  recvn(SOCKET s, char* buf, int len, int flags);

public:
	void Lobby_Init();
	void Room_Init();
	void Ingame_Init();
public:

	XMFLOAT3	GetMyPlayerStartPos() { return m_tMyInfo.xmfPosition; }
	map<int, PlayerInfo>	GetOtherPlayerInfo() { return m_mapOtherPlayerInfos; }
	STOC_PlayerInfo GetRecvPlayerInfo(DWORD playerNum);
	PlayerInfo	GetMyInfo() { return m_tMyInfo; }
	void		SetMyPlayerInfo(Player* pPlayer);
	void		SetOtherPlayerInfo(list<Object*>* plstPlayer);
	
	vector<Client_Bullet> GetBullets() { return m_vBullets; }
	Client_GameEnd GetGameEnd() { return m_CLgameEnd; }


	string	LoadServerIPtxt(string filePath);
	// 이벤트 발생시 호출될 함수
	void CallEvent(int EventType, int args, ...); // ( 일어난 일, 가변인자 개수, 보낼 정보 )
	map<int, STOC_ROOM_MAKE_OK>* GetRoomInfo() { return &m_mapRooms; }
private:
	SOCKET	m_Sock;

private:
	//초기에 받아와야 할 변수
	PlayerInfo	m_tMyInfo;
private:
	//각 씬 단계의 스레드가 끝났는 지 판별

private:
	map<int, PlayerInfo>		m_mapOtherPlayerInfos;
	array<RoomPlayer, 8>		m_RoomPlayerSlots;	// Room Player Data
	map<DWORD, STOC_PlayerInfo> m_mapRecvPlayerInfos;
	vector<Client_Bullet>		m_vBullets;
	Client_GameEnd				m_CLgameEnd;
	int		m_iPlayerNum;
public:
	//Function For LobbyThread Send

	// Loading Scene
	void SendLoadingEnd();
	void SendConnectOK();

	// Lobby Scene
	void SendRoomMake_Request();
	void SendRoomJoin_Request();

	// Room Scene
	void SendRoomInfo_Request();
	void SendReadyState(bool ReadyState);
	void SendExit_Request();
	void SendTeamChange_Request(int type);
	void SendCharacterChange_Request(int type);

	// Ingame Scene
	void SendIngameInfo_Request();

	void SendMyPlayerInfo();
	void SendKeyInput(DWORD _keyInput);
	void ServerKeyInput();

	bool SendPacket(void* buffer);
	void error_display(const char* msg, int err_no);
public:
	//Function For LobbyThread Recv
	bool IsMoveToMainGame();



public:
	void recvUpdate();
	void recvProcessing(int _bytes);
	void packetProcessing(char* _packetBuffer);

private: // packets
	DWORD m_prevKey;
	char* packet_start_ptr;
	char recvBuffer[MAX_BUFFER];
	char packetBuffer[MAX_BUFFER];
	short packet_size;
	int savedPacket_size;
	map<int, STOC_ROOM_MAKE_OK> m_mapRooms;

};

