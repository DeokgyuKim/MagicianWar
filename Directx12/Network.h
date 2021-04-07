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
	//�ʱ⿡ �޾ƿ;� �� ����
	DWORD	m_dwClientNum;
	DWORD	m_dwTeamNum;
private:
	//�� �� �ܰ��� �����尡 ������ �� �Ǻ�
	bool	m_bLobbyEnd;
private:
	map<DWORD, OtherPlayerInfo> m_mapOtherPlayerInfos;
public:
	//Function For LobbyThread
	bool IsMoveToMainGame();
	void RecvOtherPlayerInfo();
};

