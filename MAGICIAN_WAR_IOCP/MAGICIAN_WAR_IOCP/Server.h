#pragma once
#include "framework.h"

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
