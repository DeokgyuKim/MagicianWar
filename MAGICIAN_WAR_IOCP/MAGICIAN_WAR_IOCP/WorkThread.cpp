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
		// GQCS 작업
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

		if (key == EVENT_KEY) {

		}
		else { // 이때 key로 넘어오는 값이 user_num 값
			if (g_Clients[key]->Socket == NULL) { // key로 넘어온 socket가 비었으면 Accept안됨
				continue;
			}

			clientSock = g_Clients[key]->Socket;

			if (io_size == 0 && over_ex->OpType != OP_ACCEPT) { // 클라이언트와 연결이 끊어짐 , Accept에서도 0일 가능성이 있다.
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
					cur_packet_size = *(short*)(&buffer[0]);
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
							g_Rooms[room_num]->roomEvent_push(rEvent);
						}
					}
					io_size -= (cur_packet_size - pr_data_size);
					buffer += (cur_packet_size - pr_data_size);
					cur_packet_size = 0;
					pr_data_size = 0;
				}
				else
				{
					g_Client_mutex.lock();
					memcpy(g_Clients[key]->Packet_buf + pr_data_size, buffer, io_size);
					g_Client_mutex.unlock();
					cur_packet_size += io_size;
					io_size = 0;
				}
			}
			
			g_Clients[key]->Cur_packet_size = cur_packet_size;
			g_Clients[key]->Prev_data_size = pr_data_size;
			

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
			int room_num = *reinterpret_cast<int*>(over_ex->Iocp_buf);
			g_Rooms[room_num]->SendRoundTime();
			delete over_ex;
			break;
		}
		case OP_ROOM_SHOPPING_TIME:
		{
			int room_num = *reinterpret_cast<int*>(over_ex->Iocp_buf);
			g_Rooms[room_num]->SendLeftShoppingTime();
			delete over_ex;
			break;
		}
		case OP_ROOM_RESET:
		{
			int room_num = *reinterpret_cast<int*>(over_ex->Iocp_buf);
			g_Rooms[room_num]->SendRoundResetTime();
			delete over_ex;
			break;
		}
		break;
		case OP_ROOM_SEND_PACKET: // 방에서 일어난 일 보내
		{
			int room_num = *reinterpret_cast<int*>(over_ex->Iocp_buf);
			g_Rooms[room_num]->Send_sendEvent_Packet();
			delete over_ex;
		}
		break;
		case OP_ROOM_UPDATE: // 방 업데이트
		{
			int room_num = *reinterpret_cast<int*>(over_ex->Iocp_buf);
			g_Rooms[room_num]->Update();
			
			delete over_ex;
		}
		break;
		//case OP_ROOM_BREAK:
		//{
		//	int room_num = *reinterpret_cast<int*>(over_ex->Iocp_buf);
		//	BreakRoom(room_num);
		//	delete over_ex;
		//	break;
		//}

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
	int playerCount = 1;
	if (room_num != NO_ROOM) { // 방에 소속되어 있다면
		g_Rooms[room_num]->ExitRoom(client_Num);
		playerCount = g_Rooms[room_num]->Get_Player_Count();
		if (playerCount <= 0) {
			BreakRoom(room_num);
		}
	}

	g_Client_mutex.lock();
	ZeroMemory(g_Clients[client_Num], sizeof(CLIENT_INFO));
	g_Clients[client_Num]->IsConnected = false;
	g_Clients[client_Num]->Socket = NULL;
	g_ConnectedClients_Number.erase(client_Num);
	g_Client_mutex.unlock();



}

void WorkThread::BreakRoom(int room_Num)
{
	if (g_Rooms[room_Num] == nullptr)
		return;

	
	g_Room_mutex.lock();
	Room* deleteRoom = g_Rooms.find(room_Num)->second;
	//deleteRoom->Release();
	delete deleteRoom;
	deleteRoom = nullptr;
	g_Rooms.erase(room_Num);
	g_Room_mutex.unlock();

	cout << room_Num << " 방이 지워졌습니다.\n";

	Server::GetInstance()->SendRoomBreak_Packet(room_Num); // 방이 없어졌으면 모든 클라이언트에게 알려줘야지

}

ROOM_EVENT WorkThread::packetProcessing(int id, void* buffer)
{
	char* packet = reinterpret_cast<char*>(buffer);
	unsigned char packetType = packet[2]; // short(size) 0 1, (char) type
	ROOM_EVENT RoomPacket;
	RoomPacket.playerID = id;
	RoomPacket.type = NO_ROOM_PACKET;

	switch (packetType)
	{
	case ctos_LoadingEnd:
		cout << "로딩 끝이래\n";
		break;
	case ctos_Connect_OK:
		cout << id << " Client Connect OK\n";
		Server::GetInstance()->SendAcceptOK(id);
		break;
		// Lobby
		case ctos_Room_List_Request:
		{
			for (int i = ROOM_INDEX_START; i < ROOM_INDEX_START + MAX_ROOMS; ++i) {
				g_Room_mutex.lock();
				if (g_Rooms[i] != nullptr) {
					Server::GetInstance()->SendRoomList(id, i);
				}
				g_Room_mutex.unlock();
			}
			break;
		}
	case ctos_Room_Make_Request:
	{
		cout << id << " - Client가 방 생성 요청을 했습니다.\n";
		int room_num = NO_ROOM;
		for (int i = ROOM_INDEX_START; i < ROOM_INDEX_START + MAX_ROOMS; ++i) {
			g_Room_mutex.lock();
			if (g_Rooms[i] == nullptr) { // i번방이 없으면 i번방 생성
				g_Rooms[i] = new Room(i, id);
				g_Rooms[i]->EnterRoom(id, true);
				Server::GetInstance()->SendRoomMake_OK_Packet(id, i); // id 가 i 번방을 만들었다.
				g_Room_mutex.unlock();
				break;
			}
			room_num = i + 1;
			g_Room_mutex.unlock();
		}
		if (room_num >= MAX_ROOMS) { // 최대 방 개수 초과
			Server::GetInstance()->SendRoomMake_Deny_Packet(id);
		}

	}
	break;
	case ctos_Room_Join_Request:
	{
		cout << id << " - Client가 방 들어가기 요청을 했습니다.\n";
		CTOS_ROOM_JOIN_REQUEST* data = reinterpret_cast<CTOS_ROOM_JOIN_REQUEST*>(buffer);
		int room_num = NO_ROOM;
		if (g_Rooms[data->room_num] != nullptr) { // 방이 존재
			if (g_Rooms[data->room_num]->EnterRoom(id)) { // 방에 입장
				Server::GetInstance()->SendRoomJoin_OK_Packet(id); // 방에 입장 성공 패킷 보내기
			}
			else {
				Server::GetInstance()->SendRoomJoin_Deny_Packet(id);
			}
		}
		else { // 방 못들어감 졸려..
			Server::GetInstance()->SendRoomJoin_Deny_Packet(id);
		}

		break;
	}
	// Room
	case ctos_RoomInfo_Request:
	{
		RoomPacket.type = ctos_RoomInfo_Request;
		break;
	}
	case ctos_Ready_OK:
	{
		cout << id << " - ready ok\n";
		RoomPacket.type = ctos_Ready_OK;
		break;
	}
	case ctos_Ready_NONE:
	{
		cout << id << " - ready no\n";
		RoomPacket.type = ctos_Ready_NONE;
		break;
	}
	case ctos_Game_Start:
	{
		cout << id << " - Request Game Start\n";
		RoomPacket.type = ctos_Game_Start;
		break;
	}
	case ctos_Team_Change:
	{
		cout << id << " - Request TeamChange\n";
		CTOS_TEAMSELECT_REQUEST* data = reinterpret_cast<CTOS_TEAMSELECT_REQUEST*>(packet);
		RoomPacket.type = ctos_Team_Change;
		RoomPacket.data1 = data->teamType;
		break;
	}
	case ctos_Room_Exit:
	{
		cout << id << " - Request Room Exit\n";
		int room_num = g_Clients[id]->Room_num;
		g_Rooms[room_num]->ExitRoom(id);
		if (g_Rooms[room_num]->Get_Player_Count() <= 0) {
			BreakRoom(room_num);
		}
		break;
	}
	case ctos_Character_Change:
	{
		cout << id << " - Request Character Change\n";
		CTOS_CHARACTER_CHANGE* data = reinterpret_cast<CTOS_CHARACTER_CHANGE*>(packet);
		RoomPacket.type = ctos_Character_Change;
		RoomPacket.data1 = data->characterType;
		break;
	}
	// Stage Scene
	case ctos_IngameInfo_Request:
	{
		cout << id << " - Request InGameInfo\n";
		RoomPacket.type = ctos_IngameInfo_Request;
		break;
	}
	case ctos_keyInput:
	{
		//cout << id << " - KeyInput\n";
		CTOS_KEYINPUT* data = reinterpret_cast<CTOS_KEYINPUT*>(packet);
		RoomPacket.type = ctos_keyInput;
		RoomPacket.data1 = data->key;
		break;
	}
	case ctos_Camera_update:
	{
		CTOS_CAMERA* data = reinterpret_cast<CTOS_CAMERA*>(packet);
		RoomPacket.type = ctos_Camera_update;
		RoomPacket.fdata1 = data->CameraX;
		RoomPacket.fdata2 = data->CameraY;
		break;
	}
	case ctos_AttackEnd:
	{
		CTOS_ATTACKEND* data = reinterpret_cast<CTOS_ATTACKEND*>(packet);
		RoomPacket.type = ctos_AttackEnd;
		RoomPacket.bdata1 = data->isAttacked;
		break;
	}
	case ctos_ShoppingStart_Request:
	{
		RoomPacket.type = ctos_ShoppingStart_Request;
		break;
	}
	case ctos_skill_Request:
	{
		CTOS_Skill* data = reinterpret_cast<CTOS_Skill*>(packet);
		RoomPacket.type = ctos_skill_Request;
		RoomPacket.ucType1 = data->skill_type;
		RoomPacket.xmfPosition = data->xmfPosition;
		RoomPacket.xmfRotate = data->xmfRotate;
		break;
	}
	default:
		cout << id << " 가 이상한"<< (int)packetType <<" Packet을 요청하였습니다.\n";
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
