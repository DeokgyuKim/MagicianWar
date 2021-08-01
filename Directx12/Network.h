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
	
	vector<Client_Bullet> GetBullets() { return m_vBullets; }
	Client_GameEnd GetGameEnd() { return m_CLgameEnd; }
	DWORD		GetCurScene() { return m_SceneChange; }

	string	LoadServerIPtxt(string filePath);
	void ClearNetworkForNext();
	// 이벤트 발생시 호출될 함수
	void CallEvent(int EventType, int args, ...); // ( 일어난 일, 가변인자 개수, 보낼 정보 )
private:
	SOCKET	m_Sock;

private:
	//초기에 받아와야 할 변수
	PlayerInfo	m_tMyInfo;
private:
	//각 씬 단계의 스레드가 끝났는 지 판별
	bool	m_bLobbyEnd;
	bool	m_LateInit;
private:
	map<DWORD, PlayerInfo>		m_mapOtherPlayerInfos;
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

	void SendReadyState(int ReadyState);
	void SendMyPlayerInfo();
	void SendKeyInput();

	bool SendPacket(void* buffer);
	void error_display(const char* msg, int err_no);
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
	

	char* packet_start_ptr;
	char recvBuffer[MAX_BUFFER];
	char packetBuffer[MAX_BUFFER];
	short packet_size;
	int savedPacket_size;

	int m_SceneChange;
	bool mainSceneLateInit;

};

