#include "Room.h"
#include "Player.h"
#include "PlayerFSM.h"
#include "Server.h"

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
	m_BlueTeam_Count = 0;
	m_RedTeam_Count = 0;
	m_curPlayer_Count = 0;
	m_isGameStart = false;
	m_istEnterable = true; // 들어올 수 있음
	m_prev_time = chrono::system_clock::now();

	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_roomPlayerSlots[i].characterType = ELEMENT_FIRE;
		m_roomPlayerSlots[i].ishost = false;
		m_roomPlayerSlots[i].used = false;
		m_roomPlayerSlots[i].readyState = false;
		m_roomPlayerSlots[i].id = NO_PLAYER;
		m_roomPlayerSlots[i].slot_num = i;
	}




	EVENT Room_Update;
	Room_Update.Object_ID = EVENT_KEY;
	Room_Update.Target_ID = m_Info.Room_Num;
	Room_Update.wakeup_time = chrono::system_clock::now() + chrono::milliseconds(16);
	Room_Update.opType = OP_ROOM_UPDATE;
	Server::GetInstance()->AddTimer(Room_Update);

}

void Room::ReInit()
{
	m_isGameStart = false;
	if (m_curPlayer_Count <= MAX_PLAYER) { // 게임 끝났으니 꽉찬거 아니면 더 받자
		m_istEnterable = true;
	}
	m_prev_time = chrono::system_clock::now();

	m_player_mutex.lock();

	for (auto& player : m_players) {
		player->ReInit();
	}
	m_player_mutex.unlock();
}

void Room::Release()
{
	for (auto& player : m_players)
	{
		if (player != nullptr)
			delete player;
		player = nullptr;
	}
	m_players.clear();
}

void Room::Update()
{
	if (this == nullptr) return;

	//cout << "방 업데이트\n";
	m_start_time = chrono::system_clock::now();
	m_elapsedTime = m_start_time - m_prev_time;
	m_prev_time = m_start_time;

	float elapsedTime = m_elapsedTime.count() * 1000.f;
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
	for (auto player : m_players) {
		player->Update(fTime);
	}

	///PhysXUpdate
	CPhysXMgr::GetInstance()->gScene->simulate(fTime);
	CPhysXMgr::GetInstance()->gScene->fetchResults(true);

	for (auto player : m_players) {
		player->LateUpdate(fTime);
		Push_UpdatePlayerInfoPacket(player);
	}

}

void Room::Physics_Collision()
{
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
			if (i <= 3) { // Blue Team
				_player->setPosition(XMFLOAT3(20.f, 0.f, 10.f));
				_player->setTeam(TEAM_BLUE);
			}
			else if (i <= 7) { // Red Team
				_player->setPosition(XMFLOAT3(30.f, 0.f, 90.f));
				_player->setTeam(TEAM_RED);
			}
			m_players.emplace_back(_player);
			PushGameStartEvent(m_roomPlayerSlots[i].id);

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
	m_player_mutex.lock();
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
	m_player_mutex.unlock();


	g_Client_mutex.lock();
	g_Clients[id]->Room_num = NO_ROOM;
	g_Client_mutex.unlock();
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
	case ctos_Camera_y:
	{
		for (auto player : m_players) {
			if (player->getID() == rEvent.playerID) {
				XMFLOAT3 xmfRotate = player->getRotate();
				xmfRotate.y = rEvent.fdata;
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
	}
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

void Room::PushRoomPlayerEvent(int roomSlot_num)
{
	if (this == nullptr) return;
	STOC_ROOM_CHANGE packet;
	m_player_mutex.lock();
	packet.size = sizeof(packet);
	packet.type = stoc_RoomPlayer_Change;
	packet.used = m_roomPlayerSlots[roomSlot_num].used;
	packet.characterType = m_roomPlayerSlots[roomSlot_num].characterType;
	packet.host = m_roomPlayerSlots[roomSlot_num].ishost;
	packet.readyState = m_roomPlayerSlots[roomSlot_num].readyState;
	packet.slot_num = m_roomPlayerSlots[roomSlot_num].slot_num;
	packet.id = m_roomPlayerSlots[roomSlot_num].id;
	m_player_mutex.unlock();
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
	m_player_mutex.lock();
	packet.size = sizeof(packet);
	packet.type = stoc_RoomPlayer_Change;
	packet.used = m_roomPlayerSlots[roomSlot_num].used;
	packet.characterType = m_roomPlayerSlots[roomSlot_num].characterType;
	packet.host = m_roomPlayerSlots[roomSlot_num].ishost;
	packet.readyState = m_roomPlayerSlots[roomSlot_num].readyState;
	packet.slot_num = m_roomPlayerSlots[roomSlot_num].slot_num;
	packet.id = m_roomPlayerSlots[roomSlot_num].id;
	m_player_mutex.unlock();
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

void Room::PushIngame_PlayerInfo_Start(int id)
{
	if (this == nullptr) return;

	STOC_INGAME_STARTINFO packet;
	packet.size = sizeof(packet);
	packet.type = stoc_InGame_StartInfo;
	m_player_mutex.lock();
	for (auto player : m_players) {
		packet.CharacterType = player->getCharacterType();
		packet.dwTeamNum = player->getTeam();
		packet.iHp = player->getHp();
		packet.xmfPosition = player->getPosition();
		packet.id = player->getID();
		sendEvent_push(id, &packet);
	}
	m_player_mutex.unlock();
}

void Room::Push_SceneChange(int id, char sceneType)
{
	STOC_sceneChange packet;
	packet.size = sizeof(packet);
	packet.type = stoc_SceneChange;
	packet.sceneType = sceneType;
	sendEvent_push(id, &packet);
}

void Room::Push_UpdatePlayerInfoPacket(Player* _player)
{
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

	for (auto player : m_players) {
		sendEvent_push(player->getID(), &packet);
	}
}

