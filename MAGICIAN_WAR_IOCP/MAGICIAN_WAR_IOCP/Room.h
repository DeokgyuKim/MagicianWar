#pragma once
#include "framework.h"
#include "protocol.h"
#include "Global.h"
class Player;

class Room
{
public:
	Room(int room_num, int host);
	~Room();

public:
	void Initalize(int room_num,int host);
	void ReInit();
	void Update();
	void Physics_Collision();

public:
	bool EnterRoom(int id);
	void ExitRoom(int id);
	bool BreakRoom();

	void roomEvent_push(ROOM_EVENT& rEvent);
	void recvEvent_Copy();
	void recvEvnet_Clear();
	void packet_processing(ROOM_EVENT rEvent);
	
	void RoomInfo_push(int id, void* buffer);
	void Send_RoomInfo_Packet();
	


public:
	// Get
	bool isGameStart() { return m_isGameStart; }
	bool isEnterable() { return m_istEnterable; }
	int Get_Room_Num() { return m_Info.Room_Num; }
	
private:
	RoomInfo m_Info;	// room_num , room_name
	
	array<Player*, MAX_PLAYER> m_players;

	queue<ROOM_EVENT>	m_recvEventQueue;	 // 처리할 이벤트 모아놓기
	queue<ROOM_EVENT>	m_recvEventQueue_Copied;
	mutex				m_recv_mutex;

	queue<Send_Packet_RoomInfo>	m_sendEventQueue; // 보낼 이벤트 모아놓기
	mutex				m_send_mutex;


	chrono::system_clock::time_point	m_start_time;
	chrono::system_clock::time_point	m_prev_time;
	chrono::duration<float>				m_elapsedTime;

	bool m_isGameStart;
	bool m_istEnterable;

	int	m_BlueTeam_Count;
	int m_RedTeam_Count;
	int m_curPlayer_Count;

};

