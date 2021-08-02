#include "Server.h"
#include "ThreadMgr.h"
#include "extern.h"
#include "protocol.h"
#include <MSWSock.h>

#pragma comment(lib, "mswsock.lib")


Server* Server::m_pInstance = NULL;

Server::Server()
{
	ZeroMemory(&m_ServerAddress, sizeof(SOCKADDR_IN));

}

void Server::Release()
{
	ThreadMgr::GetInstance()->Threads_Join();

	for (int user_num = 0; user_num < MAX_CLIENTS; ++user_num) {
		delete g_Clients[user_num];
		g_Clients[user_num] = nullptr;
	}

	WSACleanup();
}

void Server::Initialize()
{
	std::wcout.imbue(std::locale("korean"));

	WinSock_Init();

	g_IOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); // IOCP
	m_listenSocket = TCPSocket_Create();
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(*(m_listenSocket.get())), g_IOCP, SERVER_KEY, 0);
	Socket_Bind();
	Socket_Listen();


}

void Server::MainThread_Run()
{
	// MainThread는 Client Accept 전담
	ThreadMgr::GetInstance()->Threads_Create();

	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EX Accept_over;
	ZeroMemory(&Accept_over.Over, sizeof(Accept_over.Over));
	Accept_over.OpType = OP_ACCEPT;
	Accept_over.Wsabuf.len = static_cast<int>(clientSocket);
	AcceptEx(*(m_listenSocket.get()), clientSocket, Accept_over.Iocp_buf,
		NULL, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &Accept_over.Over);

	DWORD flags = 0;
	int user_num;
	int ret;
	SOCKET clientSocket_accept;

	while (true)
	{
		user_num = NO_PLAYER;
		g_Accept_mutex.lock();
		while (g_Accept_queue.empty()) { // Accept할게 없으면 쓰레드 중지 ( 이러면 코어가 쓰레드할당 안받지 않을까 )
			g_Accept_mutex.unlock();
			//cout << "Accept 대기\n";
			this_thread::sleep_for(10ms);
			g_Accept_mutex.lock();
		}

		clientSocket_accept = g_Accept_queue.top();
		
		g_Accept_queue.pop();
		
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (!g_Clients[i]->IsConnected) { // 연결 안된곳에다가 클라연결
				user_num = i;
				break;
			}
		}

		

		g_Accept_mutex.unlock();




		CLIENT_INFO* newPlayer = new CLIENT_INFO;
		newPlayer->Cur_packet_size = 0;
		newPlayer->IsConnected = true;
		newPlayer->Socket = clientSocket_accept;
		newPlayer->Prev_data_size = 0;
		newPlayer->Recv_over.Wsabuf.len = MAX_BUFFER;
		newPlayer->Recv_over.Wsabuf.buf = reinterpret_cast<CHAR*>(newPlayer->Recv_over.Iocp_buf);
		newPlayer->Recv_over.OpType = OP_RECV;
		newPlayer->Room_num = NO_ROOM;
		newPlayer->User_num = user_num;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket_accept), g_IOCP, user_num, 0);


		g_Accept_mutex.lock();
		g_Clients[user_num] = newPlayer;
		memset(&g_Clients[user_num]->Recv_over.Over, 0, sizeof(WSAOVERLAPPED));
		ret = WSARecv(g_Clients[user_num]->Socket, &g_Clients[user_num]->Recv_over.Wsabuf,
			1, NULL, &flags, &g_Clients[user_num]->Recv_over.Over, NULL);
		g_Accept_mutex.unlock();
		if (ret == SOCKET_ERROR) {
			int err_no = WSAGetLastError();
			if (err_no != ERROR_IO_PENDING) {
				error_display("WSARecv() - ", err_no);
			}
		}

		cout << user_num << " - Accept Success\n";
		g_Accept_mutex.lock();
		g_ConnectedClients_Number.insert({ user_num ,user_num });
		g_Accept_mutex.unlock();
		

		clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		ZeroMemory(&Accept_over.Over, sizeof(Accept_over.Over));
		Accept_over.OpType = OP_ACCEPT;
		Accept_over.Wsabuf.len = static_cast<int>(clientSocket);
		AcceptEx(*(m_listenSocket.get()), clientSocket, Accept_over.Iocp_buf,
			NULL, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &Accept_over.Over);
	}
}

bool Server::SendPacket(int id, void* buffer)
{
	char* packet = reinterpret_cast<char*>(buffer);
	short packetSize = (short)packet[0];
	OVER_EX* over = new OVER_EX;
	memset(over, 0, sizeof(OVER_EX));
	over->OpType = OP_SEND;
	memcpy(over->Iocp_buf, packet, packetSize);
	over->Wsabuf.len = packetSize;
	over->Wsabuf.buf = reinterpret_cast<CHAR*>(over->Iocp_buf);

	int ret = WSASend(g_Clients[id]->Socket, &over->Wsabuf, 1, 0, 0, &over->Over, 0);
	if (ret == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			error_display("SendPacket() - ", err_no);
			return false;
		}
	}
	return true;
}

void Server::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러 - " << err_no << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}

void Server::SendAcceptOK(int id)
{
	STOC_Accept_OK packet;
	packet.size = sizeof(packet);
	packet.type = stoc_Accept_OK;
	packet.id = id;
	
	if (!SendPacket(id, &packet)) {
		cout << "SendAcceptOK() Failed \n";
	};
}

void Server::SendRoomMake_OK_Packet(int host_num, int room_num)
{
	STOC_ROOM_MAKE_OK packet;
	packet.size = sizeof(packet);
	packet.type = stoc_Room_Make_OK;
	packet.Host_num = host_num;
	packet.room_num = room_num;

	for (auto iter = g_ConnectedClients_Number.begin(); iter != g_ConnectedClients_Number.end(); ++iter) {
		if (!SendPacket(iter->second, &packet)) {
			cout << "SendRoomMake_OK_Packet() Failed \n";
		};
	}

}

void Server::SendRoomMake_Deny_Packet(int id)
{
	STOC_ROOM_MAKE_DENY packet;
	packet.size = sizeof(packet);
	packet.type = stoc_Room_Make_Deny;

	if (!SendPacket(id, &packet)) {
		cout << "SendRoomMake_Deny_Packet() Failed \n";
	};
}

void Server::SendRoomJoin_OK_Packet(int id)
{
}

void Server::SendRoomJoin_Deny_Packet(int id)
{
}


bool Server::WinSock_Init()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_WSAData) != 0) {
		int err_no = WSAGetLastError();
		error_display("WSAStartup()", err_no);
		return false;
	}
	return true;
}

bool Server::Socket_Bind()
{
	m_ServerAddress.sin_family = AF_INET;
	m_ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	m_ServerAddress.sin_port = htons(SERVER_PORT);

	int retval = bind(*(m_listenSocket.get()), reinterpret_cast<SOCKADDR*>(&m_ServerAddress), sizeof(m_ServerAddress));
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		error_display("Socket_Bind()", err_no);
		return false;
	}

	return true;
}

bool Server::Socket_Listen()
{
	int retval = listen(*(m_listenSocket.get()), SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		error_display("Socket_Listen()", err_no);
		return false;
	}

	return true;
}

unique_ptr<SOCKET> Server::TCPSocket_Create()
{
	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (socket == INVALID_SOCKET)
	{
		int err_no = WSAGetLastError();
		error_display("WSASocket()", err_no);
		return nullptr;
	}
	unique_ptr<SOCKET> socket_ptr = make_unique<SOCKET>(socket);
	return move(socket_ptr);

}
