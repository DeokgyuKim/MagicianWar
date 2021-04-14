#pragma once
#include "framework.h"

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <winsock2.h>

class Network
{
private:
	Network() {}
	~Network() {}
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
	bool GetLobbyEnd() { return m_bLobbyEnd; }
private:
	SOCKET	m_Sock;
	HANDLE	m_hRecvThreadForLobby;
private:
	//초기에 받아와야 할 변수
	DWORD	m_dwClientNum;
	DWORD	m_dwTeamNum;
private:
	//각 씬 단계의 스레드가 끝났는 지 판별
	bool	m_bLobbyEnd;
private:
	map<DWORD, PlayerInfo> m_mapOtherPlayerInfos;
	map<DWORD, SendToClientPlayerInfo> m_mapRecvPlayerInfos;
public:
	//Function For LobbyThread Send
	void SendReadyState();
	void SendMyPlayerInfo();
	void SendKeyInput();

public:
	//Function For LobbyThread Recv
	bool IsMoveToMainGame();
	void RecvOtherPlayerInfo();
	void RecvMyPlayerInfo();
};

