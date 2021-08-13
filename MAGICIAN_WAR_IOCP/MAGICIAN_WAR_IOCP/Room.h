#pragma once
#include "framework.h"
#include "protocol.h"
#include "Global.h"
#include "Bullet.h"
#include "Skill.h"
#include "FireWall.h"

class Bullet;
class Player;

class Room
{
public:
	Room(int room_num, int host);
	~Room();

public:
	void Initalize(int room_num,int host);
	void ReInit();
	void RoundStart();
	void RoundSetting();
	void Release();
	void Update();
	void InGame_Update(float fTime);
	void Physics_Collision();
	bool CheckRoundEnd(int TeamCount);
	void CheckWinnerTeam();

public:
	bool EnterRoom(int id,bool host = false);
	void ExitRoom(int id);

	void InGame_Init();
	void ResetSkill();

	void roomslot_Clear(int roomSlot_num);
	void Player_Disconnect(int id);

	void roomEvent_push(ROOM_EVENT& rEvent);
	void recvEvent_Copy();
	void recvEvnet_Clear();
	void packet_processing(ROOM_EVENT rEvent);
	
	void sendEvent_push(int id, void* buffer);
	void Send_sendEvent_Packet();
	
public:
	void PushExitPlayer(int id,int out_player_id);
	void PushRoomPlayerEvent(int roomSlot_num);
	void PushRoomPlayerEvent_Byid(int id, int roomSlot_num);
	void PushGameStartEvent(int id);
	void PushRoundStartEvent(int Cur_Round);
	void PushIngame_PlayerInfo_Start(int id);
	void Push_SceneChange(int id,char sceneType);
	void Send_UpdatePlayerInfoPacket(Player* _player);

	void PushBullet_Update(int Bullet_Index);
	void PushBullet_Delete(int Bullet_Index);
	
	void PushAddKillPoint(int id);

	void PushRoundEndEvent(int TeamType);
	void SendLeftShoppingTime();
	void SendRoundTime();
	void SendRoundResetTime();
	void PushRoundReset();
	
	void PushSkillCreate(int slotNum, unsigned char SkillType);
	void PushSkillUpdate(int slotNum, unsigned char SkillType);
	void PushSkillDelete(int slotNum, unsigned char SkillType);
public:
	// Get
	bool isGameStart() { return m_isGameStart; }
	bool isEnterable() { return m_istEnterable; }
	int Get_Room_Num() { return m_Info.Room_Num; }
	int Get_Player_Count() { return m_curPlayer_Count; }
	
private:
	RoomInfo m_Info;	// room_num , room_name
	bool m_isRoundEnd;
	bool m_isRoundStart;
	bool m_isShoppingStart;
	bool m_RoundWinnerCheck;
	bool m_isRoundReset;

	int	m_WinnerTeam;
	mutex m_player_mutex;
	list<Player*> m_players;
	array<Bullet, MAX_BULLET> m_Bullets;
	array<FireWall, MAX_SKILL> m_FireWall_Skills;
	mutex m_bullet_mutex;
	mutex m_physic_mutex;

	array<RoomPlayer, MAX_PLAYER> m_roomPlayerSlots;

	queue<ROOM_EVENT>	m_recvEventQueue;	 // 처리할 이벤트 모아놓기
	queue<ROOM_EVENT>	m_recvEventQueue_Copied;
	mutex				m_recv_mutex;
	mutex				m_recvCopied_mutex;

	queue<Send_Packet_RoomInfo>	m_sendEventQueue; // 보낼 이벤트 모아놓기
	mutex				m_send_mutex;


	chrono::system_clock::time_point	m_start_time;
	chrono::system_clock::time_point	m_prev_time;
	chrono::duration<float>				m_elapsedTime;

	unsigned char m_ShoppingTime;
	unsigned char m_TotalShoppingTime;
	unsigned char m_ResetTime;
	unsigned char m_TotalRestTime;
	short m_RoundTime;
	short m_TotalRoundTime;
	bool m_isGameStart;
	bool m_istEnterable;

	int	m_BlueTeam_Alive_Count;
	int m_RedTeam_Alive_Count;
	int m_curPlayer_Count;

};

