#include "WorkThread.h"
#include "Server.h"
#include "extern.h"
#include "protocol.h"
WorkThread::WorkThread()
{
	memset(&m_ServerAddress, 0, sizeof(SOCKADDR_IN));
}

void WorkThread::Initialize()
{
	m_ServerAddress = Server::GetInstance()->Get_ServerAddress();

	m_Thread = thread([&]()
	{
		WorkThread::Thread_Run();
	});
}

void WorkThread::Thread_Run()
{
	// Running Thread
	while (true) {
		// GQCS �۾�
		DWORD io_size;
		ULONG_PTR key;
		WSAOVERLAPPED* over;

		int ret = GetQueuedCompletionStatus(g_IOCP, &io_size, &key, &over, INFINITE);
		if (ret == FALSE) {
			//int err_no = GetLastError();
			//Server::GetInstance()->error_display("GQCS()", err_no);
		}

		SOCKET clientSock;
		OVER_EX* over_ex = reinterpret_cast<OVER_EX*>(over);

		if (key == SERVER_KEY) {

		}
		else { // �̶� key�� �Ѿ���� ���� user_num ��
			if (g_Clients[key]->Socket == NULL) { // key�� �Ѿ�� socket�� ������� Accept�ȵ�
				continue;
			}

			clientSock = g_Clients[key]->Socket;

			if (io_size == 0 && over_ex->OpType != OP_ACCEPT) { // Ŭ���̾�Ʈ�� ������ ������ , Accept������ 0�� ���ɼ��� �ִ�.
				Disconnect_Client(key, clientSock);
				continue;
			}

		}
		switch (over_ex->OpType)
		{
		case OP_RECV:
		{
			char* buffer = g_Clients[key]->Recv_over.Iocp_buf;
			unsigned int cur_packet_size = g_Clients[key]->Cur_packet_size;
			unsigned int pr_data_size = g_Clients[key]->Prev_data_size;
			ROOM_EVENT rEvent{ NO_PLAYER, NO_ROOM_PACKET };

			while (io_size > 0)
			{
				if (cur_packet_size == 0) {
					cur_packet_size = (short)buffer[0];
				}
				if (io_size + pr_data_size >= cur_packet_size) {
					unsigned char packet[MAX_PACKET];
					memcpy(packet, g_Clients[key]->Packet_buf, cur_packet_size);
					memcpy(packet + pr_data_size, buffer, cur_packet_size - pr_data_size);
					rEvent = packetProcessing(key, packet);

					if (rEvent.type != NO_ROOM_PACKET) {
						if (g_Clients[key]->Room_num == NO_ROOM) {
							cout << key << " - NO_ROOM && ROOM_PACKET\n";
						}
						else {
							int room_num = g_Clients[key]->Room_num;
							// ���� �� ��������..
						}
					}
					io_size -= (cur_packet_size - pr_data_size);
					buffer += (cur_packet_size - pr_data_size);
					cur_packet_size = 0;
					pr_data_size = 0;
				}
				else
				{
					g_Clients[key]->Client_mutex.lock();
					memcpy(g_Clients[key]->Packet_buf + pr_data_size, buffer, io_size);
					g_Clients[key]->Client_mutex.unlock();
					cur_packet_size += io_size;
					io_size = 0;
				}
			}
			g_Clients[key]->Client_mutex.lock();
			g_Clients[key]->Cur_packet_size = cur_packet_size;
			g_Clients[key]->Prev_data_size = pr_data_size;
			g_Clients[key]->Client_mutex.unlock();

			DWORD flags = 0;
			memset(&over_ex->Over, 0, sizeof(WSAOVERLAPPED));
			over_ex->OpType = OP_RECV;

			int ret = WSARecv(clientSock, &over_ex->Wsabuf,
				1, NULL, &flags, &over_ex->Over, 0);
			if (ret == SOCKET_ERROR) {
				int err_no = WSAGetLastError();
				if (err_no != ERROR_IO_PENDING) {
					cout << "Recv Loop Error \n";
					Server::GetInstance()->error_display("WSARecv() - ", err_no);
				}
			}
		}
		break;
		case OP_ACCEPT:
		{
			Accept_Client(static_cast<SOCKET>(over_ex->Wsabuf.len));
		}
		break;
		case OP_SEND:
		{
			delete over_ex;
		}
		break;
		case OP_ROOM_TIME:
		{

		}
		break;
		case OP_ROOM_SEND_PACKET:
		{

		}
		break;
		case OP_ROOM_UPDATE:
		{

		}
		break;

		}
	}
}

void WorkThread::Join()
{
	m_Thread.join();
}

void WorkThread::Disconnect_Client(int client_Num, SOCKET client)
{
	int room_num = g_Clients[client_Num]->Room_num;

	if (room_num != NO_ROOM) { // �濡 �ҼӵǾ� �ִٸ�

	}

	g_Client_mutex.lock();
	ZeroMemory(g_Clients[client_Num], sizeof(CLIENT_INFO));
	g_Clients[client_Num]->IsConnected = false;
	g_Clients[client_Num]->Socket = NULL;
	g_ConnectedClients_Number.erase(client_Num);
	g_Client_mutex.unlock();
}

ROOM_EVENT WorkThread::packetProcessing(int id, void* buffer)
{
	char* packet = reinterpret_cast<char*>(buffer);
	char packetType = packet[2]; // short(size) 0 1, (char) type
	ROOM_EVENT RoomPacket;
	RoomPacket.playerID = id;
	RoomPacket.type = NO_ROOM;

	switch (packetType)
	{
	case ctos_LoadingEnd:
		cout << "�ε� ���̷�\n";
		break;
	case ctos_Connect_OK:
		cout << id << " Client Connect OK\n";
		Server::GetInstance()->SendAcceptOK(id);
		break;
		// Lobby
	case ctos_Room_Make_Request:
	{
		cout << id << " - Client�� �� ���� ��û�� �߽��ϴ�.\n";
		int room_num = NO_ROOM;
		for (int i = 0; i < MAX_ROOMS; ++i) {
			if (g_Rooms[i] == nullptr) { // i������ ������ i���� ����
				g_Rooms[i] = new Room(i,id);
				g_Rooms[i]->EnterRoom(id);
				Server::GetInstance()->SendRoomMake_OK_Packet(id, i);
				break;
			}
			room_num = i + 1;
		}
		if (room_num >= MAX_ROOMS) { // �ִ� �� ���� �ʰ�
			Server::GetInstance()->SendRoomMake_Deny_Packet(id);
		}

	}
	break;
	case ctos_Room_Join_Request:
	{
		cout << id << " - Client�� �� ���� ��û�� �߽��ϴ�.\n";
		CTOS_ROOM_JOIN_REQUEST* data = reinterpret_cast<CTOS_ROOM_JOIN_REQUEST*>(buffer);
		int room_num = NO_ROOM;
		if (g_Rooms[data->room_num] != nullptr) { // ���� ����
			if (g_Rooms[data->room_num]->isEnterable()) { // ���尡��?

			}

		}
		else { // �� ���� ����..
			Server::GetInstance()->SendRoomJoin_Deny_Packet(id);
		}

	}
	default:
		break;
	}

	return RoomPacket;
}

void WorkThread::Accept_Client(SOCKET client)
{
	g_Accept_mutex.lock();
	g_Accept_queue.push(client);
	g_Accept_mutex.unlock();
}
