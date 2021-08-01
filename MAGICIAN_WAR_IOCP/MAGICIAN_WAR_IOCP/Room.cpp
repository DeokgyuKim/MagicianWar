#include "Room.h"
#include "Player.h"
#include "Server.h"

Room::Room(int room_num)
{
	Initalize(room_num);
}

Room::~Room()
{
}

void Room::Initalize(int room_num)
{
	m_Info.Room_Name = "";
	m_Info.Room_Num = room_num;
	m_BlueTeam_Count = 0;
	m_RedTeam_Count = 0;
	m_isGameStart = false;
	m_prev_time = chrono::system_clock::now();


	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_players[i] = new Player;
	}

}

void Room::ReInit()
{
	m_isGameStart = false;
	m_prev_time = chrono::system_clock::now();
	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_players[i]->ReInit(); // 재설정
	}
}

void Room::Update()
{
	m_start_time = chrono::system_clock::now();
	m_elapsedTime = m_start_time - m_prev_time;
	m_prev_time = m_start_time;

	float elapsedTime = m_elapsedTime.count();
	recvEvent_Copy();

	ROOM_EVENT rEvent;
	while (!m_recvEventQueue_Copied.empty())
	{
		rEvent = m_recvEventQueue_Copied.front();
		m_recvEventQueue_Copied.pop();

		packet_processing(rEvent);
	}

	if (m_isGameStart)  // 게임 시작되면 처리할 부분
	{
		// player처리         
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int id = m_players[i]->getID();
			if (id != -1)
			{
				m_players[i]->Update(elapsedTime);
			}
		}
	}
}

void Room::Physics_Collision()
{
}

bool Room::EnterRoom(int id, int playerNum)
{
	return false;
}

void Room::ExitRoom(int id)
{
}

bool Room::BreakRoom()
{
	return false;
}

void Room::roomEvent_push(ROOM_EVENT& rEvent)
{
	m_recv_mutex.lock();
	m_recvEventQueue.push(rEvent);
	m_recv_mutex.unlock();
}

void Room::recvEvent_Copy()
{
	m_recv_mutex.lock();
	m_recvEventQueue_Copied = m_recvEventQueue;
	while (!m_recvEventQueue.empty()) {
		m_recvEventQueue.pop();
	}
	m_recv_mutex.unlock();
}

void Room::recvEvnet_Clear()
{
	while (!m_recvEventQueue_Copied.empty()) {
		m_recvEventQueue_Copied.pop();
	}
}

void Room::packet_processing(ROOM_EVENT rEvent)
{
	switch (rEvent.type)
	{
	case ctos_Ready_OK:
	{
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_players[i]->getID() == rEvent.playerID) {
				m_players[i]->setReady(true);

				bool ready = m_players[i]->getReady();

				break;
			}
		}
		break;
	}
	case ctos_Ready_NONE:
	{
		break;
	}
	default:
		break;
	}
}

void Room::RoomInfo_push(int id, void* buffer)
{
	char* packet = reinterpret_cast<char*>(buffer);
	Send_Packet_RoomInfo SP_RoomInfo;
	SP_RoomInfo.playerID = id;
	memcpy(&SP_RoomInfo.buffer, buffer, (short)packet[0]);

	m_send_mutex.lock();
	m_sendEventQueue.push(SP_RoomInfo);
	m_send_mutex.unlock();
}

void Room::Send_RoomInfo_Packet()
{
	m_send_mutex.lock();

	queue<Send_Packet_RoomInfo> sendEventQueue_Copied = m_sendEventQueue;
	while (!m_sendEventQueue.empty()) 
	{
		m_sendEventQueue.pop();
	}

	m_send_mutex.unlock();

	while (!sendEventQueue_Copied.empty())
	{
		Send_Packet_RoomInfo SP_RoomInfo = sendEventQueue_Copied.front();
		sendEventQueue_Copied.pop();

		if (!Server::GetInstance()->SendPacket(SP_RoomInfo.playerID, SP_RoomInfo.buffer)) {
			cout << "Error - Send_RoomInfo_Packet()\n";
		}
	}

}

