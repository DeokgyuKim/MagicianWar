#include "Room.h"
#include "Player.h"
#include "PlayerFSM.h"
#include "Server.h"
#include "FireWall.h"


Room::Room(int room_num, int host)
{
	Initalize(room_num, host);
}

Room::~Room()
{
	Release();
}

void Room::Initalize(int room_num, int host)
{
	m_Info.Room_Name = "";
	m_Info.Room_Num = room_num;
	m_Info.HostPlayer = host;
	m_Info.TotalRound = 3;
	m_Info.curRound = 0;

	m_BlueTeam_Alive_Count = 0;
	m_RedTeam_Alive_Count = 0;
	m_curPlayer_Count = 0;
	m_isGameStart = false;
	m_istEnterable = true; // 들어올 수 있음
	m_isRoundReset = false;
	m_prev_time = chrono::system_clock::now();

	m_TotalShoppingTime = 0;//SHOPPING_TIME;
	m_ShoppingTime = 0;
	m_TotalRestTime = 5;
	m_ResetTime = 0;
	m_TotalRoundTime = 300;
	m_RoundTime = 0;


	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_roomPlayerSlots[i].characterType = ELEMENT_FIRE;
		m_roomPlayerSlots[i].ishost = false;
		m_roomPlayerSlots[i].used = false;
		m_roomPlayerSlots[i].readyState = false;
		m_roomPlayerSlots[i].id = NO_PLAYER;
		m_roomPlayerSlots[i].slot_num = i;
	}

	for (int i = 0; i < MAX_SKILL; ++i) {
		m_FireWall_Skills[i].setUser(NO_PLAYER);
		m_FireWall_Skills[i].setSlotNum(i);
	}

	m_isRoundEnd = false;
	m_isRoundStart = false;
	m_isShoppingStart = false;
	m_RoundWinnerCheck = false;
	m_WinnerTeam = TEAM_NONE;

	EVENT Room_Update;
	Room_Update.Object_ID = EVENT_KEY;
	Room_Update.Target_ID = m_Info.Room_Num;
	Room_Update.wakeup_time = chrono::system_clock::now() + chrono::milliseconds(16);
	Room_Update.opType = OP_ROOM_UPDATE;
	Server::GetInstance()->AddTimer(Room_Update);

}

void Room::ReInit()
{	// 라운드가 다 끝나고 로비로 돌아옴
	m_isGameStart = false;
	m_Info.curRound = 0;
	m_WinnerTeam = TEAM_NONE;

	if (m_curPlayer_Count <= MAX_PLAYER) { // 게임 끝났으니 꽉찬거 아니면 더 받자
		m_istEnterable = true;
	}
	m_prev_time = chrono::system_clock::now();

	m_player_mutex.lock();

	// m_players는 인게임에서만 씀
	for (auto& player : m_players)
	{
		if (player != nullptr)
			delete player;
		player = nullptr;
	}
	m_players.clear();

	m_player_mutex.unlock();

	// Bullet초기화
	for (int i = 0; i < BulletCB_Count; ++i) {
		if (m_Bullets[i].getUser() != NO_PLAYER)
			m_Bullets[i].Release();

		m_Bullets[i].SetUser(NO_PLAYER);
	}

	ResetSkill();

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_roomPlayerSlots[i].used) {// 사용중인 슬릇만 레디 해제
			m_roomPlayerSlots[i].readyState = false;
		}
	}

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_roomPlayerSlots[i].used) {
			Push_SceneChange(m_roomPlayerSlots[i].id, ROOM_SCENE);

		}
	}

}

void Room::RoundStart()
{
	m_isRoundEnd = false;
	
	PushRoundStartEvent(++m_Info.curRound);
	m_RoundTime = 0;
	SendRoundTime();

}

void Room::RoundSetting()
{
	m_WinnerTeam = TEAM_NONE;
	// 상점 끝나고 라운드 시작할때 해줄것
	for (auto player : m_players) { // 초기 위치 재설정
		int spawnPos = player->getSlotNum();
		cout << "spawnPos - " << spawnPos << "\n";
		if (spawnPos == 0) player->setPosition(XMFLOAT3(20.f, 0.f, 10.f));
		else if (spawnPos == 1) player->setPosition(XMFLOAT3(15.f, 0.f, 10.f));
		else if (spawnPos == 2) player->setPosition(XMFLOAT3(10.f, 0.f, 10.f));
		else if (spawnPos == 3) player->setPosition(XMFLOAT3(5.f, 0.f, 10.f));
		else if (spawnPos == 4) player->setPosition(XMFLOAT3(40.f, 0.f, 90.f));
		else if (spawnPos == 5) player->setPosition(XMFLOAT3(45.f, 0.f, 90.f));
		else if (spawnPos == 6) player->setPosition(XMFLOAT3(50.f, 0.f, 90.f));
		else if (spawnPos == 7) player->setPosition(XMFLOAT3(55.f, 0.f, 90.f));

		player->setHp(100);
		player->SetRotate(XMFLOAT3(0.f, 0.f, 0.f));
		player->GetUpperFSM()->ChangeState(STATE_IDLE, ANIM_IDLE);
		player->GetRootFSM()->ChangeState(STATE_IDLE, ANIM_IDLE);
	}

	// Bullet초기화
	for (int i = 0; i < BulletCB_Count; ++i) {
		if (m_Bullets[i].getUser() != NO_PLAYER)
			m_Bullets[i].Release();

		m_Bullets[i].SetUser(NO_PLAYER);
	}

	ResetSkill();

}

void Room::Release()
{
	// Player
	for (auto& player : m_players)
	{
		if (player != nullptr)
			delete player;
		player = nullptr;
	}
	m_players.clear();

	for (int i = 0; i < BulletCB_Count; ++i) {
		if (m_Bullets[i].getUser() != NO_PLAYER)
			m_Bullets[i].Release();
	}

}

void Room::Update()
{
	if (this == nullptr) return;

	//cout << "방 업데이트\n";
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
		//if(m_isRoundEnd)
		InGame_Update(elapsedTime);

	}
	else // 게임 시작이 아니면 RoomScene이니까
	{

	}

	chrono::system_clock::time_point end_time = chrono::system_clock::now();
	chrono::duration<float> timePassed = end_time - m_start_time;

	int callUpdate = static_cast<int>((0.016f - timePassed.count()) * 1000.f);
	if (callUpdate <= 0)
		callUpdate = 0;

	EVENT Room_Update;
	Room_Update.Object_ID = EVENT_KEY;
	Room_Update.Target_ID = m_Info.Room_Num;
	Room_Update.wakeup_time = chrono::system_clock::now() + chrono::milliseconds(callUpdate);
	Room_Update.opType = OP_ROOM_UPDATE;
	Server::GetInstance()->AddTimer(Room_Update);

	EVENT roomEvent_Send;
	roomEvent_Send.Object_ID = EVENT_KEY;
	roomEvent_Send.Target_ID = m_Info.Room_Num;
	roomEvent_Send.wakeup_time = chrono::system_clock::now() + chrono::milliseconds(1);
	roomEvent_Send.opType = OP_ROOM_SEND_PACKET;
	Server::GetInstance()->AddTimer(roomEvent_Send);



}

void Room::InGame_Update(float fTime)
{
	// 인게임 player 처리
	Bullet BulletTemp;
	//if(m_isRoundEnd)
	for (auto player : m_players)
	{

		player->Update(fTime);


		if (player->getCreateBullet() == 1)
		{
			BulletTemp.SetUser(player->getID());
			BulletTemp.setCheckUserTeam(player->getTeam());
			BulletTemp.setElementType(player->getCharacterType());
			BulletTemp.setWorldMatrix(player->getBulletStartWorld());
			BulletTemp.setPosition(player->getPosition());
			BulletTemp.setTotalLifeTime(5.f);
			BulletTemp.setDirection(XMFLOAT3{ -player->getWorld()._21,-player->getWorld()._22 ,-player->getWorld()._23 });

			for (int i = 0; i < BulletCB_Count; ++i) {
				if (m_Bullets[i].getUser() == NO_PLAYER) { // 안쓰는 총알 찾아서

					m_Bullets[i] = BulletTemp;

					player->setCreateBullet(0);
					break;
				}
			}
		}
	}

	// 총알 Update
	for (int i = 0; i < BulletCB_Count; ++i) {

		if (m_Bullets[i].getUser() != NO_PLAYER) { // 사용중인 것만 update
			int dead = m_Bullets[i].Update(fTime);
			if (dead) { // 총알이 사망체크되면 모든 클라한테 알려줘야함
				m_Bullets[i].SetUser(NO_PLAYER);
				PushBullet_Delete(i);
			}
			else {
				m_Bullets[i].LateUpdate(fTime);
				PushBullet_Update(i);
			}

		}

	}

	for (int i = 0; i < MAX_SKILL; ++i) {
		if (m_FireWall_Skills[i].getUser() != NO_PLAYER) {
			int dead = m_FireWall_Skills[i].Update(fTime);
			if (dead) {
				m_FireWall_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_FIREWALL);
			}
			else {
				m_FireWall_Skills[i].LateUpdate(fTime);
				PushSkillUpdate(i, SKILL_FIREWALL);
			}
		}
		if (m_IceBall_Skills[i].getUser() != NO_PLAYER) {
			bool dead = m_IceBall_Skills[i].Update(fTime);
			if (dead) {
				m_IceBall_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_COLD1);
			}
			else {
				m_IceBall_Skills[i].LateUpdate(fTime);
				PushSkillUpdate(i, SKILL_COLD1);
			}
		}
	}


	if (CPhysXMgr::GetInstance()->PhysXStartTime > 10.f)
	{
		CPhysXMgr::GetInstance()->gScene->simulate(fTime);
		CPhysXMgr::GetInstance()->gScene->fetchResults(true);
	}
	else
	{
		CPhysXMgr::GetInstance()->PhysXStartTime += fTime;
	}

	///PhysXUpdate
	//CPhysXMgr::GetInstance()->gScene->simulate(fTime);
	//CPhysXMgr::GetInstance()->gScene->fetchResults(true);

	if (!m_isRoundEnd) {

		Physics_Collision();
		CheckWinnerTeam();
	}
	else { // 라운드가 끝났을때
		SendRoundResetTime();
	}

	for (auto player : m_players) {

		player->LateUpdate(fTime);

		Send_UpdatePlayerInfoPacket(player);
	}



}

void Room::Physics_Collision()
{
	//총알 충돌
	for (int i = 0; i < BulletCB_Count; ++i)
	{
		if (m_Bullets[i].getUser() != NO_PLAYER)
		{

			if (CPhysXMgr::GetInstance()->CollisionForStaticObjects(m_Bullets[i].GetRigidDynamic()))
			{
				m_Bullets[i].SetUser(NO_PLAYER);

				PushBullet_Delete(i);
			}
			for (auto player : m_players)
			{
				// 같은 팀이 쏜 총이 아니고 죽은 플레이어가 아니고 승자가 없을때
				if (m_Bullets[i].getCheckUserTeam() != player->getTeam()
					&& player->getState() != STATE_DEAD && m_WinnerTeam == TEAM_NONE)
				{
					if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(player->GetPxCapsuleController()->getActor(), m_Bullets[i].GetRigidDynamic()))
					{
						int Attack_Player = m_Bullets[i].getUser();
						m_Bullets[i].SetUser(NO_PLAYER);
						PushBullet_Delete(i);
						//플레이어 피달고 그런거
						player->setDamage(m_Bullets[i].getDamage());
						if (player->getHp() <= 0)
						{
							PushAddKillPoint(Attack_Player); // 1킬 했음 너가
							player->GetUpperFSM()->ChangeState(STATE_DEAD, ANIM_DEAD);
							player->GetRootFSM()->ChangeState(STATE_DEAD, ANIM_DEAD);
							if (player->getTeam() == TEAM_BLUE) { // 죽은 친구가 BlueTeam이면
								--m_BlueTeam_Alive_Count;
								m_isRoundEnd = CheckRoundEnd(m_BlueTeam_Alive_Count);
							}
							else if (player->getTeam() == TEAM_RED) {
								--m_RedTeam_Alive_Count;
								m_isRoundEnd = CheckRoundEnd(m_RedTeam_Alive_Count);
							}
							if (m_isRoundEnd) // 라운드가 끝나면
							{
								if (player->getTeam() == TEAM_RED) { // 마지막에 죽은 친구팀 파악
									m_WinnerTeam = TEAM_BLUE;
								}
								else if (player->getTeam() == TEAM_BLUE) {
									m_WinnerTeam = TEAM_RED;
								}
								m_RoundWinnerCheck = true;
							}
						}
						else
							player->GetUpperFSM()->ChangeState(STATE_HIT, ANIM_HIT);
					}
				}

			}
		}
	}
}

bool Room::CheckRoundEnd(int TeamCount)
{
	if (TeamCount <= 0) {
		return true;
	}
	return false;
}

void Room::CheckWinnerTeam()
{
	if (m_RoundWinnerCheck) // 라운드가 끝났을때
	{
		if (m_WinnerTeam == TEAM_BLUE) // 승자팀이 나오면
		{
			PushRoundEndEvent(TEAM_BLUE);
			for (auto player : m_players) {
				if (player->getTeam() == TEAM_BLUE)
				{
					player->GetUpperFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
					player->GetRootFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
				}
			}
			m_RoundWinnerCheck = false;
		}
		else if (m_WinnerTeam == TEAM_RED)
		{
			PushRoundEndEvent(TEAM_RED);
			for (auto player : m_players) {
				if (player->getTeam() == TEAM_RED)
				{
					player->GetUpperFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
					player->GetRootFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
				}
			}
			m_RoundWinnerCheck = false;
		}
	}
}

bool Room::EnterRoom(int id, bool host)
{
	if (m_curPlayer_Count >= MAX_PLAYER)
		return false;
	if (m_isGameStart) // 게임중
		return false;
	if (!m_istEnterable) // 들어올 수 없는 상태
		return false;

	cout << id << " 가 방에 들어옴\n";


	g_Clients[id]->Room_num = m_Info.Room_Num;


	int sendSlot = -1;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (!m_roomPlayerSlots[i].used) {
			m_roomPlayerSlots[i].used = true; // 사용중으로 바꾸고
			m_roomPlayerSlots[i].id = id;
			m_roomPlayerSlots[i].slot_num = i; // 내 슬릇넘버 기억해야함
			m_roomPlayerSlots[i].ishost = host;
			sendSlot = i;


			break;
		}
	}


	++m_curPlayer_Count;

	PushRoomPlayerEvent(sendSlot); // 룸 플레이어가 변화될때마다 호출할 것
	Server::GetInstance()->SendRoomEnter(id, m_Info.Room_Num); // room_num 방에 id가 들어감


	//for (int i = 0; i < MAX_PLAYER; ++i) { // 룸에 접속한 모든 플레이어에게 보내줘야해 룸데이터 밀어줘야해
	//	if (m_roomPlayerSlots[i].used) {
	//		Server::GetInstance()->SendRoomPlayerInfo(m_roomPlayerSlots[i].id, i);
	//	}
	//
	//}
	return true;
}

void Room::ExitRoom(int id)
{
	int islotNum = -1;
	int CharacterSlot = -1;
	bool ishost = false;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_roomPlayerSlots[i].id == id) { // 나가는 친구 체크
			ishost = m_roomPlayerSlots[i].ishost;
			islotNum = i;
			break;
		}
	}


	if (ishost) { // 나간 플레이어가 host면 다른 플레이어가 host가 된다.
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_roomPlayerSlots[i].used && m_roomPlayerSlots[i].id != id) {
				m_roomPlayerSlots[i].ishost = true;
				PushRoomPlayerEvent(i);
				break;
			}
		}
	}

	Player_Disconnect(id);
	roomslot_Clear(islotNum);
	PushRoomPlayerEvent(islotNum);

	for (auto player : m_players) {
		int receiver = player->getID();
		PushExitPlayer(receiver, id);
	}

	--m_curPlayer_Count;

}

void Room::InGame_Init()
{
	m_isGameStart = true;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_roomPlayerSlots[i].used) {
			PushRoomPlayerEvent(i);
			Player* _player = new Player(m_roomPlayerSlots[i].id, m_Info.Room_Num);
			_player->setHp(100);
			_player->setCharacterType(m_roomPlayerSlots[i].characterType);
			_player->setSlotNum(m_roomPlayerSlots[i].slot_num);
			if (i <= 3) { // Blue Team
				if (i == 0)_player->setPosition(XMFLOAT3(20.f, 0.f, 10.f));
				else if (i == 1)_player->setPosition(XMFLOAT3(15.f, 0.f, 10.f));
				else if (i == 2)_player->setPosition(XMFLOAT3(10.f, 0.f, 10.f));
				else if (i == 3)_player->setPosition(XMFLOAT3(5.f, 0.f, 10.f));

				++m_BlueTeam_Alive_Count;
				_player->setTeam(TEAM_BLUE);
			}
			else if (i <= 7) { // Red Team
				if (i == 4)_player->setPosition(XMFLOAT3(40.f, 0.f, 90.f));
				else if (i == 5)_player->setPosition(XMFLOAT3(45.f, 0.f, 90.f));
				else if (i == 6)_player->setPosition(XMFLOAT3(50.f, 0.f, 90.f));
				else if (i == 7)_player->setPosition(XMFLOAT3(55.f, 0.f, 90.f));

				++m_RedTeam_Alive_Count;
				_player->setTeam(TEAM_RED);
			}
			m_players.emplace_back(_player);
			PushGameStartEvent(m_roomPlayerSlots[i].id);

		}
	}


}

void Room::ResetSkill()
{
	for (int i = 0; i < MAX_SKILL; ++i) 
	{
		if (m_FireWall_Skills[i].getUser() != NO_PLAYER)
		{
			m_FireWall_Skills[i].setUser(NO_PLAYER);
		}
		if (m_IceBall_Skills[i].getUser() != NO_PLAYER)
		{
			m_IceBall_Skills[i].setUser(NO_PLAYER);
		}
	}
}


void Room::roomslot_Clear(int roomSlot_num)
{
	m_roomPlayerSlots[roomSlot_num].characterType = ELEMENT_FIRE;
	m_roomPlayerSlots[roomSlot_num].ishost = false;
	m_roomPlayerSlots[roomSlot_num].used = false;
	m_roomPlayerSlots[roomSlot_num].readyState = false;
	m_roomPlayerSlots[roomSlot_num].id = NO_PLAYER;
	m_roomPlayerSlots[roomSlot_num].slot_num = roomSlot_num;

}

void Room::Player_Disconnect(int id)
{
	if (this == nullptr) return;
	//m_player_mutex.lock();
	for (auto iter = m_players.begin(); iter != m_players.end();)
	{
		if ((*iter)->getID() == id) {
			if ((*iter) != nullptr)
				delete* iter;
			*iter = nullptr;
			iter = m_players.erase(iter);
		}
		else
			++iter;
	}
	//m_player_mutex.unlock();


	g_Client_mutex.lock();
	g_Clients[id]->Room_num = NO_ROOM;
	g_Client_mutex.unlock();

	if (g_Clients[id]->IsConnected) // connect되어 있냐 얘?
		Server::GetInstance()->SendRoomExit(id);

}

void Room::roomEvent_push(ROOM_EVENT& rEvent)
{
	if (this == nullptr) return;

	m_recv_mutex.lock();
	m_recvEventQueue.push(rEvent);
	m_recv_mutex.unlock();
}

void Room::recvEvent_Copy()
{
	if (this == nullptr) return;

	m_recvCopied_mutex.lock();
	m_recvEventQueue_Copied = m_recvEventQueue;
	while (!m_recvEventQueue.empty()) {
		m_recvEventQueue.pop();
	}
	m_recvCopied_mutex.unlock();
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
	case ctos_keyInput:
	{
		for (auto player : m_players) {
			if (player->getID() == rEvent.playerID) {
				dynamic_cast<PlayerFSM*>(player->GetUpperFSM())->SetDefaultKey(rEvent.data1);
				dynamic_cast<PlayerFSM*>(player->GetRootFSM())->SetDefaultKey(rEvent.data1);
				break;
			}
		}
		break;
	}
	case ctos_CreateBullet_Request:
	{
		for (auto player : m_players) {
			if (player->getID() == rEvent.playerID) {
				cout << "서버 총알 쏴줘\n";
				player->setCreateBullet(1);
				break;
			}
		}
		break;
	}
	case ctos_RoomInfo_Request:
	{
		for (int i = 0; i < MAX_PLAYER; ++i) {
			PushRoomPlayerEvent_Byid(rEvent.playerID, i); // playerID 에게 i번 슬릇의 정보 보내기
		}
		break;
	}
	case ctos_Ready_OK:
	{
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_roomPlayerSlots[i].id == rEvent.playerID) {
				m_roomPlayerSlots[i].readyState = true;
				PushRoomPlayerEvent(i);

				break;
			}
		}
		break;
	}
	case ctos_Ready_NONE:
	{
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_roomPlayerSlots[i].id == rEvent.playerID) {
				m_roomPlayerSlots[i].readyState = false;
				PushRoomPlayerEvent(i);
				break;
			}
		}
		break;
	}
	case ctos_Game_Start:
	{
		bool gameStart = true;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_roomPlayerSlots[i].used) { // 사용중인 슬릇에
				if (!m_roomPlayerSlots[i].ishost) { // 방장이 아닌 친구가
					if (!m_roomPlayerSlots[i].readyState) { // 레디안한 친구가 있으면
						gameStart = false;
						break;
					}
				}
			}
		}
		if (gameStart) {
			cout << "게임 시작\n";
			InGame_Init();
		}
		break;
	}
	case ctos_Team_Change:
	{
		RoomPlayer Roomdata;
		int clearSlot;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_roomPlayerSlots[i].id == rEvent.playerID) {
				Roomdata = m_roomPlayerSlots[i];
				clearSlot = i;
				break;
			}
		}
		roomslot_Clear(clearSlot);

		if (rEvent.data1 == TEAM_BLUE) {
			for (int i = 0; i < 4; ++i) { // 0 ~ 4  BlueTeam
				if (!m_roomPlayerSlots[i].used) {
					Roomdata.slot_num = i;
					m_roomPlayerSlots[i] = Roomdata;
					break;
				}
			}
		}
		else if (rEvent.data1 == TEAM_RED) {
			for (int i = 4; i < 8; ++i) { // 0 ~ 4  BlueTeam
				if (!m_roomPlayerSlots[i].used) {
					Roomdata.slot_num = i;
					m_roomPlayerSlots[i] = Roomdata;
					break;
				}
			}
		}

		if (clearSlot != Roomdata.slot_num)  // 
		{
			PushRoomPlayerEvent(clearSlot); // 비워진 Slot
			PushRoomPlayerEvent(Roomdata.slot_num); // 채워진 Slot
		}

		break;
	}
	case ctos_Character_Change:
	{
		int Sendslot;
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_roomPlayerSlots[i].id == rEvent.playerID) {
				Sendslot = i;
				m_roomPlayerSlots[i].characterType = rEvent.data1;
				break;
			}
		}
		PushRoomPlayerEvent(Sendslot);
		break;
	}
	// InGame
	case ctos_IngameInfo_Request:
	{
		for (auto player : m_players) {
			PushIngame_PlayerInfo_Start(player->getID());
		}

		Push_SceneChange(rEvent.playerID, STAGE_SCENE);
		break;
	}
	case ctos_Camera_update:
	{
		for (auto player : m_players) {
			if (player->getID() == rEvent.playerID) {
				XMFLOAT3 xmfRotate = player->getRotate();
				xmfRotate.y = rEvent.fdata2;
				player->setCamera(rEvent.fdata1, rEvent.fdata2);
				player->SetRotate(xmfRotate);
				break;
			}
		}
		break;
	}
	case ctos_AttackEnd:
	{
		for (auto player : m_players) {
			if (player->getID() == rEvent.playerID) {
				player->setAttackEnd(rEvent.bdata1);
				break;
			}
		}
		break;
	}
	case ctos_ShoppingStart_Request:
	{

		recvEvnet_Clear(); // 라운드 시작전에 받아온 데이터 비워야지
		m_isRoundStart = true; // 라운드 시작 일단 시켜주고
		m_ShoppingTime = 0;
		m_isShoppingStart = true;
		SendLeftShoppingTime();
		break;
	}
	case ctos_skill_Request:
		for (auto player : m_players) 
		{
			if (player->getID() == rEvent.playerID) 
			{
				for (int i = 0; i < MAX_SKILL; ++i)
				{
					if (m_FireWall_Skills[i].getUser() == NO_PLAYER)
					{
						if (rEvent.ucType1 == SKILL_Q)
						{
							if (player->getCharacterType() == ELEMENT_FIRE) 
							{
							FireWall fireWall{};
							m_FireWall_Skills[i] = fireWall;
							m_FireWall_Skills[i].setSkillType(SKILL_FIREWALL);
							m_FireWall_Skills[i].setUser(rEvent.playerID);
							m_FireWall_Skills[i].setPosition(rEvent.xmfPosition);
							m_FireWall_Skills[i].setRotate(rEvent.xmfRotate);
							m_FireWall_Skills[i].setTeam(player->getTeam());
							unsigned char Skilltype =  m_FireWall_Skills[i].getSkillType();
							PushSkillCreate(i, Skilltype);
							break;
							}
							else if (player->getCharacterType() == ELEMENT_COLD)
							{
								IcaAgeBall iceBall{};
								m_IceBall_Skills[i] = iceBall;
								m_IceBall_Skills[i].setSkillType(SKILL_COLD1);
								m_IceBall_Skills[i].setUser(rEvent.playerID);
								m_IceBall_Skills[i].setPosition(rEvent.xmfPosition);
								m_IceBall_Skills[i].setRotate(rEvent.xmfRotate);
								m_IceBall_Skills[i].setTeam(player->getTeam());
								unsigned char Skilltype = m_IceBall_Skills[i].getSkillType();
								PushSkillCreate(i, Skilltype);
								break;
							}
							else if (player->getCharacterType() == ELEMENT_DARKNESS)
							{
								break;
							}
						}
						else if (rEvent.ucType1 == SKILL_E)
						{
							if (player->getCharacterType() == ELEMENT_FIRE)
							{
								break;
							}
							else if (player->getCharacterType() == ELEMENT_COLD)
							{
								break;
							}
							else if (player->getCharacterType() == ELEMENT_DARKNESS)
							{
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		break;
	default:
		break;
	}
}

void Room::sendEvent_push(int id, void* buffer)
{
	if (this == nullptr) return;

	char* packet = reinterpret_cast<char*>(buffer);
	Send_Packet_RoomInfo SP_RoomInfo;
	SP_RoomInfo.playerID = id;

	memcpy(&SP_RoomInfo.buffer, buffer, *(short*)(&packet[0]));

	m_send_mutex.lock();
	m_sendEventQueue.push(SP_RoomInfo);
	m_send_mutex.unlock();
}

void Room::Send_sendEvent_Packet()
{
	if (this == nullptr) return;

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

void Room::PushExitPlayer(int id, int out_player_id)
{
	if (this == nullptr)return;

	STOC_INGAME_OUTPLAYER packet;
	packet.size = sizeof(packet);
	packet.type = stoc_Ingame_OutPlayer;
	packet.outPlayer_id = out_player_id;

	sendEvent_push(id, &packet);
}

void Room::PushRoomPlayerEvent(int roomSlot_num)
{
	if (this == nullptr) return;
	STOC_ROOM_CHANGE packet;

	packet.size = sizeof(packet);
	packet.type = stoc_RoomPlayer_Change;
	packet.used = m_roomPlayerSlots[roomSlot_num].used;
	packet.characterType = m_roomPlayerSlots[roomSlot_num].characterType;
	packet.host = m_roomPlayerSlots[roomSlot_num].ishost;
	packet.readyState = m_roomPlayerSlots[roomSlot_num].readyState;
	packet.slot_num = m_roomPlayerSlots[roomSlot_num].slot_num;
	packet.id = m_roomPlayerSlots[roomSlot_num].id;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_roomPlayerSlots[i].used) { // 사용중
			int id = m_roomPlayerSlots[i].id;
			sendEvent_push(id, &packet);
		}
	}

}

void Room::PushRoomPlayerEvent_Byid(int id, int roomSlot_num)
{
	if (this == nullptr) return;

	STOC_ROOM_CHANGE packet;

	packet.size = sizeof(packet);
	packet.type = stoc_RoomPlayer_Change;
	packet.used = m_roomPlayerSlots[roomSlot_num].used;
	packet.characterType = m_roomPlayerSlots[roomSlot_num].characterType;
	packet.host = m_roomPlayerSlots[roomSlot_num].ishost;
	packet.readyState = m_roomPlayerSlots[roomSlot_num].readyState;
	packet.slot_num = m_roomPlayerSlots[roomSlot_num].slot_num;
	packet.id = m_roomPlayerSlots[roomSlot_num].id;

	sendEvent_push(id, &packet);
}

void Room::PushGameStartEvent(int id)
{
	if (this == nullptr) return;

	STOC_GAME_START packet;
	packet.size = sizeof(packet);
	packet.type = stoc_Game_Start;
	sendEvent_push(id, &packet);
}

void Room::PushRoundStartEvent(int Cur_Round)
{
	if (this == nullptr) return;
	STOC_ROUND_START packet;
	packet.size = sizeof(packet);
	packet.type = stoc_roundstart;
	packet.Cur_Round = Cur_Round;

	for (auto player : m_players) { // 게임에 존재하는 모든 플레이어에게 메시지를 보내줘야한다.
		int id = player->getID();
		sendEvent_push(id, &packet);
	}

}

void Room::PushIngame_PlayerInfo_Start(int id)
{
	if (this == nullptr) return;

	STOC_INGAME_STARTINFO packet;
	packet.size = sizeof(packet);
	packet.type = stoc_InGame_StartInfo;

	for (auto player : m_players) {
		packet.CharacterType = player->getCharacterType();
		packet.dwTeamNum = player->getTeam();
		packet.iHp = player->getHp();
		packet.xmfPosition = player->getPosition();
		packet.id = player->getID();
		sendEvent_push(id, &packet);
	}

}

void Room::Push_SceneChange(int id, char sceneType)
{
	if (this == nullptr) return;

	STOC_sceneChange packet;
	packet.size = sizeof(packet);
	packet.type = stoc_SceneChange;
	packet.sceneType = sceneType;
	sendEvent_push(id, &packet);
}

void Room::Send_UpdatePlayerInfoPacket(Player* _player)
{
	if (this == nullptr) return;

	STOC_PlayerInfo packet;
	packet.size = sizeof(packet);
	packet.type = stoc_playerInfo;
	packet.ePlayerState = _player->getState();
	packet.Root_eAnimType = _player->getRootAnimType();
	packet.Upper_eAnimType = _player->getUpperAnimType();

	//packet.playerInfo.x = _player->getWorld();
	packet.playerInfo = _player->getInfo();
	packet.bAttackEnd = _player->IsAttackEnded();
	packet.playerInfo.iHp = _player->getHp();

	m_player_mutex.lock();
	for (auto player : m_players) {
		Server::GetInstance()->SendIngamePlayerInfo(player->getID(), packet);
	}
	m_player_mutex.unlock();
}

void Room::PushBullet_Update(int Bullet_Index)
{
	if (this == nullptr) return;

	STOC_Bullet_Update packet;
	packet.size = sizeof(packet);
	packet.type = stoc_bullet_update;
	packet.ElementType = m_Bullets[Bullet_Index].getElementType();
	packet.xmmWorld = m_Bullets[Bullet_Index].getWorld();
	packet.index = Bullet_Index;

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

void Room::PushBullet_Delete(int Bullet_Index)
{
	if (this == nullptr) return;

	STOC_Bullet_Delete packet;
	packet.size = sizeof(packet);
	packet.type = stoc_bullet_delete;
	packet.index = Bullet_Index;

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}

}

void Room::PushAddKillPoint(int id)
{
	if (this == nullptr) return;
	STOC_ADD_KILL_POINT packet;
	packet.size = sizeof(packet);
	packet.type = stoc_add_kill_point;
	sendEvent_push(id, &packet);
}

void Room::PushRoundEndEvent(int TeamType)
{
	if (this == nullptr) return;

	STOC_RoundEnd packet;
	packet.size = sizeof(packet);
	packet.type = stoc_roundend;
	packet.teamType = TeamType;

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

void Room::SendLeftShoppingTime()
{
	if (this == nullptr) return;

	if (!m_isRoundReset) {
		RoundSetting();
		m_isRoundReset = true;
	}

	if (m_ShoppingTime <= m_TotalShoppingTime) {
		EVENT roomEvent_Send;
		roomEvent_Send.Object_ID = EVENT_KEY;
		roomEvent_Send.Target_ID = m_Info.Room_Num;
		roomEvent_Send.wakeup_time = chrono::system_clock::now() + chrono::seconds(1); // 1초에 한번
		roomEvent_Send.opType = OP_ROOM_SHOPPING_TIME;
		Server::GetInstance()->AddTimer(roomEvent_Send);


		unsigned char leftTime = m_TotalShoppingTime - m_ShoppingTime;
		cout << "시간 - " << (int)leftTime << "\n";
		++m_ShoppingTime;

		for (auto player : m_players) // 게임중인 플레이어들에게 쇼핑시간 보내줘야지
		{
			int id = player->getID();
			Server::GetInstance()->SendLeftTime(id, leftTime);
		}


	}
	else
	{
		cout << "라운드 시작\n";
		m_isShoppingStart = false;
		m_ShoppingTime = 0;
		RoundStart();
	}

}

void Room::SendRoundTime()
{
	if (this == nullptr) return;

	if (m_isShoppingStart) return;

	if (m_RoundTime <= m_TotalRoundTime) {
		EVENT roomEvent_Send;
		roomEvent_Send.Object_ID = EVENT_KEY;
		roomEvent_Send.Target_ID = m_Info.Room_Num;
		roomEvent_Send.wakeup_time = chrono::system_clock::now() + chrono::seconds(1); // 1초에 한번
		roomEvent_Send.opType = OP_ROOM_TIME;
		Server::GetInstance()->AddTimer(roomEvent_Send);

		short leftTime = m_TotalRoundTime - m_RoundTime;
		cout << "라운드 진행중 - " << (int)leftTime << "\n";
		++m_RoundTime;

		for (auto player : m_players) // 게임중인 플레이어들에게 쇼핑시간 보내줘야지
		{
			int id = player->getID();
			Server::GetInstance()->SendLeftTime(id, leftTime);
		}


	}
	else
	{

	}
}

void Room::SendRoundResetTime()
{
	if (this == nullptr) return;

	m_isRoundEnd = false;

	if (m_ResetTime <= m_TotalRestTime) {
		EVENT roomEvent_Send;
		roomEvent_Send.Object_ID = EVENT_KEY;
		roomEvent_Send.Target_ID = m_Info.Room_Num;
		roomEvent_Send.wakeup_time = chrono::system_clock::now() + chrono::seconds(1); // 1초에 한번
		roomEvent_Send.opType = OP_ROOM_RESET;
		Server::GetInstance()->AddTimer(roomEvent_Send);

		unsigned char leftTime = m_TotalRestTime - m_ResetTime;
		cout << "라운드 재시작 - " << (int)leftTime << "\n";
		++m_ResetTime;

		//for (auto player : m_players) // 게임중인 플레이어들에게 쇼핑시간 보내줘야지
		//{
		//	int id = player->getID();
		//	Server::GetInstance()->SendRoundResetTime(id, leftTime);
		//}


	}
	else
	{
		if (m_Info.curRound < m_Info.TotalRound) { // 라운드가 남았으면 라운드 재시작
			cout << "Win Lose 팻말 치워줘\n";
			PushRoundReset();
			m_isRoundReset = false;

		}
		else {
			cout << "게임 끝\n";
			ReInit();
		}


		m_ResetTime = 0;
	}


}

void Room::PushRoundReset()
{
	STOC_ROUND_RESET packet;
	packet.size = sizeof(packet);
	packet.type = stoc_roundreset;

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

void Room::PushSkillCreate(int slotNum, unsigned char SkillType)
{
	if (this == nullptr) return;

	STOC_Skill packet;
	packet.size = sizeof(packet);
	packet.type = stoc_skill;
	packet.slotNum = static_cast<unsigned char>(slotNum);
	packet.skillType = SkillType;
	if (SkillType == SKILL_FIREWALL)
	{
		packet.user = m_FireWall_Skills[slotNum].getUser();
		packet.xmfPosition = m_FireWall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_FireWall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_FIRE2)
	{

	}
	else if (SkillType == SKILL_COLD1)
	{
		packet.user = m_IceBall_Skills[slotNum].getUser();
		packet.xmfPosition = m_IceBall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceBall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_COLD2)
	{

	}
	else if (SkillType == SKILL_DARKNESS1)
	{

	}
	else if (SkillType == SKILL_DARKNESS2)
	{

	}

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

void Room::PushSkillUpdate(int slotNum, unsigned char SkillType)
{
	if (this == nullptr) return;

	STOC_Skill packet;
	packet.size = sizeof(packet);
	packet.type = stoc_skillUpdate;
	packet.slotNum = static_cast<unsigned char>(slotNum);
	packet.skillType = SkillType;
	if (SkillType == SKILL_FIREWALL)
	{
		packet.user = m_FireWall_Skills[slotNum].getUser();
		packet.xmfPosition = m_FireWall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_FireWall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_FIRE2)
	{

	}
	else if (SkillType == SKILL_COLD1)
	{
		packet.user = m_IceBall_Skills[slotNum].getUser();
		packet.xmfPosition = m_IceBall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceBall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_COLD2)
	{

	}
	else if (SkillType == SKILL_DARKNESS1)
	{

	}
	else if (SkillType == SKILL_DARKNESS2)
	{

	}

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

void Room::PushSkillDelete(int slotNum, unsigned char SkillType)
{
	if (this == nullptr) return;

	STOC_SKILL_DELETE packet;
	packet.size = sizeof(packet);
	packet.type = stoc_skillDelete;
	packet.skillType = static_cast<unsigned char>(SkillType);
	packet.slotNum = slotNum;

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

