#pragma once
#include "framework.h"

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <winsock2.h>

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
private:
	SOCKET	m_Sock;

private:
	//초기에 받아와야 할 변수
	PlayerInfo	m_tMyInfo;
private:
	//각 씬 단계의 스레드가 끝났는 지 판별
	bool	m_bLobbyEnd;
private:
	map<DWORD, PlayerInfo> m_mapOtherPlayerInfos;
	map<DWORD, STOC_PlayerInfo> m_mapRecvPlayerInfos;
	int		m_iPlayerNum;
public:
	//Function For LobbyThread Send
	void SendReadyState();
	void SendMyPlayerInfo();
	void SendKeyInput();

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

	DWORD m_SceneChange;
	bool mainSceneLateInit;
};

