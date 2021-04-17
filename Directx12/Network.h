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
	bool Release();
	void Update();
	int  recvn(SOCKET s, char* buf, int len, int flags);
public:
	bool		GetLobbyEnd() { return m_bLobbyEnd; }
	XMFLOAT3	GetMyPlayerStartPos() { return m_tMyInfo.xmfPosition; }
	void		SetMyPlayerInfo(Player* pPlayer);
	void		SetOtherPlayerInfo(list<Object*>* plstPlayer);
private:
	SOCKET	m_Sock;
	CRITICAL_SECTION m_Crt;
	HANDLE	m_hRecvThreadForLobby;

private:
	//�ʱ⿡ �޾ƿ;� �� ����
	PlayerInfo	m_tMyInfo;
private:
	//�� �� �ܰ��� �����尡 ������ �� �Ǻ�
	bool	m_bLobbyEnd;
private:
	map<DWORD, PlayerInfo> m_mapOtherPlayerInfos;
	map<DWORD, SendToClientPlayerInfo> m_mapRecvPlayerInfos;
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
	void RecvPlayerInfo();
};

