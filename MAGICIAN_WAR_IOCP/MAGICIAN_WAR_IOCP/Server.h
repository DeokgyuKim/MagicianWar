#pragma once
#include "framework.h"
#include "Global.h"

class Server
{
private:
	Server();
	Server(const Server& rhs) {}
	~Server() {}
public:
	static Server* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new Server();
		return m_pInstance;
	}
	static void DestoryInstance() {
		if (m_pInstance != NULL)
		{
			m_pInstance->Release();
			delete m_pInstance;
		}
		m_pInstance = NULL;
	}
private:
	static Server* m_pInstance;
private:
	void	Release();
public:
	// Get
	SOCKADDR_IN Get_ServerAddress() { return m_ServerAddress; }

	// Set

public:
	void Initialize();
	void MainThread_Run();

	bool SendPacket(int id, void* buffer);
	void error_display(const char* msg, int err_no);

	// Loading
	void SendAcceptOK(int id);

	// Lobby
	void SendRoomMake_OK_Packet(int host_num, int room_num); // ( 방장, 방 번호 )
	void SendRoomMake_Deny_Packet(int id);
	void SendRoomJoin_OK_Packet(int id);
	void SendRoomJoin_Deny_Packet(int id);
	void SendRoomBreak_Packet(int room_num);
	void SendRoomPlayerInfo(int id, int slot_num);
	void SendRoomEnter(int id, int room_num);
	void SendRoomList(int id, int room_num);

	// Room
	void SendRoomExit(int id);
	
	void AddTimer(EVENT& rEvent);
private:
	bool WinSock_Init();
	unique_ptr<SOCKET> TCPSocket_Create();
	bool Socket_Bind();
	bool Socket_Listen();




private:
	WSADATA m_WSAData;

	SOCKADDR_IN m_ServerAddress;
	unique_ptr<SOCKET>     m_listenSocket;
};
