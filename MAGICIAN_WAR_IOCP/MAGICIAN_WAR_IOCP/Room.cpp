#include "Room.h"

#include "PlayerFSM.h"
#include "Server.h"
#include "FireWall.h"
#include "extern.h"


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

	m_TotalShoppingTime = 5;//SHOPPING_TIME;
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

	SkillInit();

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
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			m_players[i].Release();
			m_players[i].setUsed(false);
		}

	}


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
	recvEvnet_Clear();
	PushRoundStartEvent(++m_Info.curRound);
	m_RoundTime = 0;
	SendRoundTime();

}

void Room::RoundSetting()
{
	recvEvnet_Clear();
	m_WinnerTeam = TEAM_NONE;
	// 상점 끝나고 라운드 시작할때 해줄것
	for (int i = 0; i < MAX_PLAYER; ++i) { // 초기 위치 재설정
		if (m_players[i].getUsed()) {
			int spawnPos = m_players[i].getSlotNum();
			//cout << "spawnPos - " << spawnPos << "\n";
			if (spawnPos == 0) m_players[i].setPosition(XMFLOAT3(20.f, 0.f, 10.f));
			else if (spawnPos == 1) m_players[i].setPosition(XMFLOAT3(15.f, 0.f, 10.f));
			else if (spawnPos == 2) m_players[i].setPosition(XMFLOAT3(10.f, 0.f, 10.f));
			else if (spawnPos == 3) m_players[i].setPosition(XMFLOAT3(5.f, 0.f, 10.f));
			else if (spawnPos == 4) m_players[i].setPosition(XMFLOAT3(40.f, 0.f, 90.f));
			else if (spawnPos == 5) m_players[i].setPosition(XMFLOAT3(45.f, 0.f, 90.f));
			else if (spawnPos == 6) m_players[i].setPosition(XMFLOAT3(50.f, 0.f, 90.f));
			else if (spawnPos == 7) m_players[i].setPosition(XMFLOAT3(55.f, 0.f, 90.f));

			m_players[i].setHp(100);
			if (spawnPos < 4)
				m_players[i].SetRotate(XMFLOAT3(0.f, 0.f, 0.f));
			else
				m_players[i].SetRotate(XMFLOAT3(0.f, 180.f, 0.f));

			m_players[i].GetUpperFSM()->ChangeState(STATE_IDLE, ANIM_IDLE);
			m_players[i].GetRootFSM()->ChangeState(STATE_IDLE, ANIM_IDLE);
			dynamic_cast<PlayerFSM*>(m_players[i].GetUpperFSM())->SetDefaultKey(0);
			dynamic_cast<PlayerFSM*>(m_players[i].GetRootFSM())->SetDefaultKey(0);

			m_players[i].setCreateBullet(0);
			m_players[i].setCreateBullet(0);
		}
	}

	// Bullet초기화
	for (int i = 0; i < BulletCB_Count; ++i) {
		//if (m_Bullets[i].getUser() != NO_PLAYER)
		//	m_Bullets[i].Release();

		//m_Bullets[i].SetUser(NO_PLAYER);
		if (m_Bullets[i].getUser() != NO_PLAYER)
			m_Bullets[i].SetDead(true);
	}

	ResetSkill();

}

void Room::Release()
{
	//// Player
	//for (int i = 0; i < MAX_PLAYER; ++i)
	//{
	//	if (m_players[i].getUsed()) {
	//		m_players[i].Release();
	//	}
	//}


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
	// PlayerUPdate
	PlayerUpdate(fTime);

	// 총알 Update
	BulletUpdate(fTime);

	// Skill Update
	SkillUpdate(fTime);

	// Physics_Simulate
	Physics_Simulate(fTime);

	// Physics_Collision & WinnerCheck
	if (!m_isRoundEnd) {
		Physics_Collision();
		CheckWinnerTeam();
	}
	else { // 라운드가 끝났을때 라운드 재시작까지의 시간
		SendRoundResetTime();
	}


	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			m_players[i].LateUpdate(fTime);
		}
	}
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			Send_UpdatePlayerInfoPacket(m_players[i]);
		}
	}



}

void Room::PlayerUpdate(float fTime)
{
	Bullet BulletTemp;

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed())
		{
			m_players[i].Update(fTime);
			if (m_players[i].getCreateBullet() == 1)
			{
				BulletTemp.SetUser(m_players[i].getID());
				BulletTemp.setCheckUserTeam(m_players[i].getTeam());
				BulletTemp.setElementType(m_players[i].getCharacterType());
				BulletTemp.setWorldMatrix(m_players[i].getBulletStartWorld());
				BulletTemp.setPosition(m_players[i].getPosition());
				BulletTemp.setTotalLifeTime(5.f);
				BulletTemp.setDirection(XMFLOAT3{ m_players[i].getDirWorld()._31,m_players[i].getDirWorld()._32 ,m_players[i].getDirWorld()._33 });

				for (int j = 0; j < BulletCB_Count; ++j)
				{
					if (m_Bullets[j].getUser() == NO_PLAYER) { // 안쓰는 총알 찾아서

						m_Bullets[j] = BulletTemp;
						break;
					}
				}
				m_players[i].GetUpperFSM()->ChangeState(STATE_ATTACK, ANIM_ATTACK);
				if (m_players[i].GetRootFSM()->GetState() == STATE_IDLE) {
					m_players[i].GetRootFSM()->ChangeState(STATE_ATTACK, ANIM_ATTACK);
				}
				m_players[i].setCreateBullet(0);
			}
		}
	}
}

void Room::BulletUpdate(float fTime)
{
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
}

void Room::SkillUpdate(float fTime)
{
	for (int i = 0; i < MAX_SKILL; ++i) {
		// FIRE
		if (m_FireWall_Skills[i].getUser() != NO_PLAYER) { // FIREWALL
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

		if (m_FireMeteor_Skills[i].getUser() != NO_PLAYER) { // METEOR
			int dead = m_FireMeteor_Skills[i].Update(fTime);

			if (dead) {
				int a;
				m_FireMeteor_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_FIRE2);
			}
			else {
				m_FireMeteor_Skills[i].LateUpdate(fTime);
				PushSkillUpdate(i, SKILL_FIRE2);
			}
		}

		// ICE
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
		if (m_IceFreeze_Skills[i].getUser() != NO_PLAYER) {
			bool dead = m_IceFreeze_Skills[i].Update(fTime);
			if (dead) {
				m_IceFreeze_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_COLD2);
			}
			else {
				m_IceFreeze_Skills[i].LateUpdate(fTime);
				PushSkillUpdate(i, SKILL_COLD2);
			}
		}

		if (m_IceCressvas_Skills[i].getUser() != NO_PLAYER) {
			bool dead = m_IceCressvas_Skills[i].Update(fTime);
			if (dead) {
				m_IceCressvas_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_CRESSVAS);
			}
			else {
				m_IceCressvas_Skills[i].LateUpdate(fTime);
				//PushSkillUpdate(i, SKILL_COLD2);
			}
		}

		// DARKNESS
		if (m_Darkness_Enchantress_Skills[i].getUser() != NO_PLAYER) {
			bool dead = m_Darkness_Enchantress_Skills[i].Update(fTime);
			for (auto& player : m_players)
			{
				if (player.getUsed() == true)
				{
					if (player.getID() == m_Darkness_Enchantress_Skills[i].getUser())
					{
						m_Darkness_Enchantress_Skills[i].setPosition(player.getPosition());
					}
				}
			}
			if (dead) {
				m_Darkness_Enchantress_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_DARKNESS1);
			}
			else {
				m_Darkness_Enchantress_Skills[i].LateUpdate(fTime);
				PushSkillUpdate(i, SKILL_DARKNESS1);
			}
		}
		if (m_Darkness_DistortionPearl_Skills[i].getUser() != NO_PLAYER) {
			bool dead = m_Darkness_DistortionPearl_Skills[i].Update(fTime);
			if (dead) {
				m_Darkness_DistortionPearl_Skills[i].setUser(NO_PLAYER);
				PushSkillDelete(i, SKILL_DARKNESS2);
			}
			else {
				m_Darkness_DistortionPearl_Skills[i].LateUpdate(fTime);
				PushSkillUpdate(i, SKILL_DARKNESS2);
			}
		}
	}
}

void Room::Physics_Simulate(float fTime)
{
	if (CPhysXMgr::GetInstance()->PhysXStartTime > 10.f)
	{
		CPhysXMgr::GetInstance()->gScene->simulate(fTime);
		CPhysXMgr::GetInstance()->gScene->fetchResults(true);
	}
	else
	{
		CPhysXMgr::GetInstance()->PhysXStartTime += fTime;
	}
}

void Room::Physics_Collision()
{
	//총알 충돌
	for (int i = 0; i < BulletCB_Count; ++i)
	{
		if (m_Bullets[i].getUser() != NO_PLAYER)
		{

			if (CPhysXMgr::GetInstance()->CollisionForStaticObjects(m_Bullets[i].GetRigidDynamic()) || m_Bullets[i].GetPosition().y <= 0.25f)
			{
				m_Bullets[i].SetUser(NO_PLAYER);

				PushBullet_Delete(i);
			}
			for (int j = 0; j < MAX_PLAYER; ++j)
			{
				if (m_players[j].getUsed()) {
					// 같은 팀이 쏜 총이 아니고 죽은 플레이어가 아니고 승자가 없을때
					if (m_Bullets[i].getCheckUserTeam() != m_players[j].getTeam()
						&& m_players[j].getState() != STATE_DEAD && m_WinnerTeam == TEAM_NONE)
					{
						if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_players[j].GetPxCapsuleController()->getActor(), m_Bullets[i].GetRigidDynamic()))
						{
							int Attack_Player = m_Bullets[i].getUser();
							m_Bullets[i].SetUser(NO_PLAYER);
							PushBullet_Delete(i);
							//플레이어 피달고 그런거
							int playerEvent = m_players[j].setDamage(m_Bullets[i].getDamage(), BULLET_HIT_EVENT);
							CheckPlayerEvent(playerEvent, j);
							if (playerEvent == PLAYER_DEAD_EVENT)
							{
								PushAddKillPoint(Attack_Player); // 1킬 했음 너가
							}
						}
					}
				}
			}
		}
	}


	for (int i = 0; i < MAX_SKILL; ++i)
	{
		if (m_IceBall_Skills[i].getUser() != NO_PLAYER)
		{ // Skill 사용자가 있을때
			for (int j = 0; j < MAX_PLAYER; ++j)
			{
				if (m_players[j].getUsed() && m_players[j].getAbleDealing())
				{ // 플레이어가 존재할때
					for (auto body : m_IceBall_Skills[i].GetRigidDynamics())
					{
						if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_players[j].GetPxCapsuleController()->getActor(), body))
						{ // 충돌 했다면
							if(m_players[j].getTeam() != m_IceBall_Skills[i].getTeamType())
							{ // 팀원 체크 + 체크하고자 하는 플레이어의 상태 체크 ( Dead, Dance, Freeze )
								if (!m_players[j].getFreeze())
								{
									int Attack_Player = m_IceBall_Skills[i].getUser();
									int playerEvent = m_players[j].setDamage(0, ICE_FIELD_HIT_EVENT);
									CheckPlayerEvent(playerEvent, j);
									if (playerEvent == PLAYER_DEAD_EVENT)
									{
										PushAddKillPoint(Attack_Player);
									}
									else if (playerEvent == ICE_FIELD_HIT_EVENT && !m_players[j].getIce())
									{
										CreateSkillCressvas(j);
										//cout << "크레바스 생성" << endl;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		for (int j = 0; j < MAX_PLAYER; ++j)
		{
			if (m_players[j].getUsed() && m_players[j].getAbleDealing())
			{
				//FireWall
	
				if (m_FireWall_Skills[i].getUser() != NO_PLAYER)
				{
					for (auto rigidbody : m_FireWall_Skills[i].GetRigidDynamics())
					{
						if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_players[j].GetPxCapsuleController()->getActor(), rigidbody))
						{
							if (m_players[j].getTeam() == m_FireWall_Skills[i].getTeamType())
							{
								if (m_players[j].getAbleHeal())
								{
									m_players[j].setHp(m_players[j].getHp() + 5);
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	
	
	//Static Object or Terrain 
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		if (m_FireMeteor_Skills[i].getUser() != NO_PLAYER)
		{
			for (auto rigidbody : m_FireMeteor_Skills[i].GetRigidDynamics())
			{
				//Static Object or Terrain
				if (CPhysXMgr::GetInstance()->CollisionForStaticObjects(rigidbody) || m_FireMeteor_Skills[i].getPosition().y <= 2.5f)
				{
					m_FireMeteor_Skills[i].setDead(true);
					for (int j = 0; j < MAX_PLAYER; ++j)
					{
						if (m_players[j].getUsed() && m_players[j].getAbleDealing())
						{
							if (m_FireMeteor_Skills[i].getTeamType() != m_players[j].getTeam())
							{
								if (!m_players[j].getFreeze())
								{
									XMFLOAT3 xmflen, playerpos, skillpos;
									playerpos = m_players[j].getPosition();
									skillpos = m_FireMeteor_Skills[i].getPosition();
									XMVECTOR length = XMVector3Length(XMLoadFloat3(&playerpos) - XMLoadFloat3(&skillpos));
									XMStoreFloat3(&xmflen, length);
									if (xmflen.x < 5.f)
									{
										int Attack_Player = m_FireMeteor_Skills[i].getUser();
										int playerEvent = m_players[j].setDamage(40, BULLET_HIT_EVENT);
										CheckPlayerEvent(playerEvent, j);
										if (playerEvent == PLAYER_DEAD_EVENT)
										{
											PushAddKillPoint(Attack_Player);
										}
									}
								}
							}
						}
					}
				}
	
				for (int j = 0; j < MAX_PLAYER; ++j)
				{
					if (m_players[j].getUsed() && m_players[j].getAbleDealing())
					{
						if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_players[j].GetPxCapsuleController()->getActor(), rigidbody))
						{
							if (!m_players[j].getFreeze())
							{
								if (m_FireMeteor_Skills[i].getTeamType() != m_players[j].getTeam())
								{
									m_FireMeteor_Skills[i].setDead(true);
									int Attack_Player = m_FireMeteor_Skills[i].getUser();
									int playerEvent = m_players[j].setDamage(40, BULLET_HIT_EVENT);
									CheckPlayerEvent(playerEvent, j);
									if (playerEvent == PLAYER_DEAD_EVENT)
									{
										PushAddKillPoint(Attack_Player);
									}
									for (int k = 0; k < MAX_PLAYER; ++k)
									{
										if (m_players[k].getUsed())
										{
											if (!m_players[k].getFreeze())
											{
												if (m_FireMeteor_Skills[i].getTeamType() != m_players[k].getTeam())
												{
													XMFLOAT3 xmflen, playerpos, skillpos;
													playerpos = m_players[k].getPosition();
													skillpos = m_FireMeteor_Skills[i].getPosition();
													XMVECTOR length = XMVector3Length(XMLoadFloat3(&playerpos) - XMLoadFloat3(&skillpos));
													XMStoreFloat3(&xmflen, length);
													if (xmflen.x < 5.f)
													{
														int Attack_Player = m_FireMeteor_Skills[i].getUser();
														int playerEvent = m_players[k].setDamage(40, BULLET_HIT_EVENT);
														CheckPlayerEvent(playerEvent, k);
														if (playerEvent == PLAYER_DEAD_EVENT)
														{
															PushAddKillPoint(Attack_Player);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		if (m_Darkness_Enchantress_Skills[i].getUser() != NO_PLAYER)
		{
			for (auto rigidbody : m_Darkness_Enchantress_Skills[i].GetRigidDynamics())
			{
				for (int j = 0; j < MAX_PLAYER; ++j)
				{
					if (m_players[j].getUsed() && m_players[j].getAbleDealing())
					{
						if (!m_players[j].getFreeze())
						{
							if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_players[j].GetPxCapsuleController()->getActor(), rigidbody))
							{
								if (m_Darkness_Enchantress_Skills[i].getTeamType() != m_players[j].getTeam())
								{
	
									Player* pPlayer = GetPlayerForID(m_Darkness_Enchantress_Skills[i].getUser());
									if (pPlayer == nullptr || !pPlayer->getUsed())
									{
										cout << "No Player." << endl;
										m_Darkness_Enchantress_Skills[i].setDead(true);
									}
									else
									{
										if (m_players[j].getAbleDottAtt())
										{
											int Attack_Player = m_FireMeteor_Skills[i].getUser();
											int playerEvent = m_players[j].setDamage(10, BULLET_HIT_EVENT);
											CheckPlayerEvent(playerEvent, j);
											if (playerEvent == PLAYER_DEAD_EVENT)
											{
												PushAddKillPoint(Attack_Player);
											}
											pPlayer->setHp(pPlayer->getHp() + 5);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		if (m_Darkness_DistortionPearl_Skills[i].getUser() != NO_PLAYER)
		{
			for (auto rigidbody : m_Darkness_DistortionPearl_Skills[i].GetRigidDynamics())
			{
				//Static Object or Terrain
				if (CPhysXMgr::GetInstance()->CollisionForStaticObjectsNotQuail(rigidbody) || m_Darkness_DistortionPearl_Skills[i].getPosition().y <= 0.5f)
				{
					m_Darkness_DistortionPearl_Skills[i].setDead(true);
					Player* pPlayer = GetPlayerForID(m_Darkness_DistortionPearl_Skills[i].getUser());
					if (pPlayer == nullptr)
					{
						cout << "No Player" << endl;
					}
					else
					{
						pPlayer->setPosition(m_Darkness_DistortionPearl_Skills[i].getPosition());
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
			for (int i = 0; i < MAX_PLAYER; ++i)
			{
				if (m_players[i].getUsed()) {
					if (m_players[i].getTeam() == TEAM_BLUE)
					{
						m_players[i].GetUpperFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
						m_players[i].GetRootFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
					}
				}
			}
			m_RoundWinnerCheck = false;
		}
		else if (m_WinnerTeam == TEAM_RED)
		{
			PushRoundEndEvent(TEAM_RED);
			for (int i = 0; i < MAX_PLAYER; ++i)
			{
				if (m_players[i].getUsed()) {
					if (m_players[i].getTeam() == TEAM_RED)
					{
						m_players[i].GetUpperFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
						m_players[i].GetRootFSM()->ChangeState(STATE_DANCE, ANIM_DANCE);
					}
				}
			}
			m_RoundWinnerCheck = false;
		}
	}
}

void Room::CheckPlayerEvent(int _playerEvent, int slotNum)
{
	if (_playerEvent == PLAYER_DEAD_EVENT) // 플레이어가 죽었을때 체크해야할것
	{
		if (m_players[slotNum].getTeam() == TEAM_BLUE) { // 죽은 친구가 BlueTeam이면
			--m_BlueTeam_Alive_Count;
			m_isRoundEnd = CheckRoundEnd(m_BlueTeam_Alive_Count);
		}
		else if (m_players[slotNum].getTeam() == TEAM_RED) {
			--m_RedTeam_Alive_Count;
			m_isRoundEnd = CheckRoundEnd(m_RedTeam_Alive_Count);
		}
		if (m_isRoundEnd) // 라운드가 끝나면
		{
			if (m_players[slotNum].getTeam() == TEAM_RED) { // 마지막에 죽은 친구팀 파악
				m_WinnerTeam = TEAM_BLUE;
			}
			else if (m_players[slotNum].getTeam() == TEAM_BLUE) {
				m_WinnerTeam = TEAM_RED;
			}
			m_RoundWinnerCheck = true;
		}
	}

}

void Room::CreateSkillCressvas(int player_slotNum)
{
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		if (m_IceCressvas_Skills[i].getUser() == NO_PLAYER)
		{
			IceCressvas IceCressvas{};
			m_IceCressvas_Skills[i] = IceCressvas;
			m_IceCressvas_Skills[i].setSkillType(SKILL_CRESSVAS);
			m_IceCressvas_Skills[i].setUser(m_players[player_slotNum].getID());
			m_IceCressvas_Skills[i].setPosition(m_players[player_slotNum].getPosition());
			m_IceCressvas_Skills[i].setRotate(m_players[player_slotNum].getRotate());
			m_IceCressvas_Skills[i].setTeam(m_players[player_slotNum].getTeam());
			unsigned char Skilltype = m_IceCressvas_Skills[i].getSkillType();
			PushSkillCreate(i, Skilltype);
			break;
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

	//cout << id << " 가 방에 들어옴\n";

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

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			int receiver = m_players[i].getID();
			PushExitPlayer(receiver, id);
		}
	}

	--m_curPlayer_Count;

}

void Room::InGame_Init()
{
	m_isGameStart = true;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (m_roomPlayerSlots[i].used) {
			PushRoomPlayerEvent(i);
			m_players[i].Initialize(m_roomPlayerSlots[i].id, m_Info.Room_Num);

			m_players[i].setHp(100);
			m_players[i].setCharacterType(m_roomPlayerSlots[i].characterType);
			m_players[i].setSlotNum(m_roomPlayerSlots[i].slot_num);
			if (i <= 3) { // Blue Team
				if (i == 0)m_players[i].setPosition(XMFLOAT3(20.f, 0.f, 10.f));
				else if (i == 1)m_players[i].setPosition(XMFLOAT3(15.f, 0.f, 10.f));
				else if (i == 2)m_players[i].setPosition(XMFLOAT3(10.f, 0.f, 10.f));
				else if (i == 3)m_players[i].setPosition(XMFLOAT3(5.f, 0.f, 10.f));

				++m_BlueTeam_Alive_Count;
				m_players[i].setTeam(TEAM_BLUE);
			}
			else if (i <= 7) { // Red Team
				if (i == 4)m_players[i].setPosition(XMFLOAT3(40.f, 0.f, 90.f));
				else if (i == 5)m_players[i].setPosition(XMFLOAT3(45.f, 0.f, 90.f));
				else if (i == 6)m_players[i].setPosition(XMFLOAT3(50.f, 0.f, 90.f));
				else if (i == 7)m_players[i].setPosition(XMFLOAT3(55.f, 0.f, 90.f));

				++m_RedTeam_Alive_Count;
				m_players[i].setTeam(TEAM_RED);
			}

			m_players[i].setUsed(true);
			PushGameStartEvent(m_roomPlayerSlots[i].id);

		}
	}


}

void Room::SkillInit()
{
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		// FIRE
		m_FireWall_Skills[i].setUser(NO_PLAYER);
		m_FireWall_Skills[i].setSlotNum(i);

		m_FireMeteor_Skills[i].setUser(NO_PLAYER);
		m_FireMeteor_Skills[i].setSlotNum(i);

		// ICE
		m_IceBall_Skills[i].setUser(NO_PLAYER);
		m_IceBall_Skills[i].setSlotNum(i);

		m_IceFreeze_Skills[i].setUser(NO_PLAYER);
		m_IceFreeze_Skills[i].setSlotNum(i);

		// DARKNESS
		m_Darkness_DistortionPearl_Skills[i].setUser(NO_PLAYER);
		m_Darkness_DistortionPearl_Skills[i].setSlotNum(i);

		m_Darkness_Enchantress_Skills[i].setUser(NO_PLAYER);
		m_Darkness_Enchantress_Skills[i].setSlotNum(i);
	}
}

void Room::ResetSkill()
{
	for (int i = 0; i < MAX_SKILL; ++i)
	{
		// FIRE
		if (m_FireWall_Skills[i].getUser() != NO_PLAYER)
		{
			m_FireWall_Skills[i].setDead(true);
		}
		if (m_FireMeteor_Skills[i].getUser() != NO_PLAYER)
		{
			m_FireMeteor_Skills[i].setDead(true);
		}

		// ICE
		if (m_IceBall_Skills[i].getUser() != NO_PLAYER)
		{
			m_IceBall_Skills[i].setDead(true);
		}
		if (m_IceFreeze_Skills[i].getUser() != NO_PLAYER)
		{
			m_IceFreeze_Skills[i].setDead(true);
		}
		if (m_IceCressvas_Skills[i].getUser() != NO_PLAYER)
		{
			m_IceCressvas_Skills[i].setDead(true);
		}

		// DARKNESS
		if (m_Darkness_Enchantress_Skills[i].getUser() != NO_PLAYER)
		{
			m_Darkness_Enchantress_Skills[i].setDead(true);
		}
		if (m_Darkness_DistortionPearl_Skills[i].getUser() != NO_PLAYER)
		{
			m_Darkness_DistortionPearl_Skills[i].setDead(true);
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

	//bool CanDisconnect = false;

	m_player_mutex.lock();

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getID() == id) {
			m_players[i].setUsed(false);
			m_players[i].Release();
			break;
		}
	}

	m_player_mutex.unlock();


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
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) {
				if (m_players[i].getID() == rEvent.playerID) {
					dynamic_cast<PlayerFSM*>(m_players[i].GetUpperFSM())->SetDefaultKey(rEvent.data1);
					dynamic_cast<PlayerFSM*>(m_players[i].GetRootFSM())->SetDefaultKey(rEvent.data1);
					break;
				}
			}
		}
		break;
	}
	case ctos_CreateBullet_Request:
	{
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) {
				if (m_players[i].getID() == rEvent.playerID) {
					//cout << "서버 총알 쏴줘\n";
					m_players[i].setCreateBullet(1);
					break;
				}
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
			//cout << "게임 시작\n";
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
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) {
				PushIngame_PlayerInfo_Start(m_players[i].getID());
			}
		}

		Push_SceneChange(rEvent.playerID, STAGE_SCENE);
		break;
	}
	case ctos_Camera_update:
	{
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) {
				if (m_players[i].getID() == rEvent.playerID) {
					XMFLOAT3 xmfRotate = m_players[i].getRotate();
					xmfRotate.y = rEvent.fdata2;
					m_players[i].setCamera(rEvent.fdata1, rEvent.fdata2);
					m_players[i].SetRotate(xmfRotate);
					break;
				}
			}
		}
		break;
	}
	case ctos_AttackEnd:
	{
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) {
				if (m_players[i].getID() == rEvent.playerID) {
					m_players[i].setAttackEnd(rEvent.bdata1);
					break;
				}
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
		for (int j = 0; j < MAX_PLAYER; ++j)
		{
			if (m_players[j].getUsed()) {
				if (m_players[j].getID() == rEvent.playerID)
				{
					for (int i = 0; i < MAX_SKILL; ++i)
					{
						if (rEvent.ucType1 == SKILL_Q)
						{
							if (m_players[j].getCharacterType() == ELEMENT_FIRE)
							{
								if (m_FireWall_Skills[i].getUser() == NO_PLAYER)
								{
									FireWall fireWall{};
									m_FireWall_Skills[i] = fireWall;
									m_FireWall_Skills[i].setSkillType(SKILL_FIREWALL);
									m_FireWall_Skills[i].setUser(rEvent.playerID);
									m_FireWall_Skills[i].setPosition(rEvent.xmfPosition);
									m_FireWall_Skills[i].setRotate(rEvent.xmfRotate);
									m_FireWall_Skills[i].setTeam(m_players[j].getTeam());
									m_FireWall_Skills[i].set_playerSlotNum(m_players[j].getSlotNum());
									m_FireWall_Skills[i].MakeCollision();
									unsigned char Skilltype = m_FireWall_Skills[i].getSkillType();
									PushSkillCreate(i, Skilltype);
									break;
								}
							}

							else if (m_players[j].getCharacterType() == ELEMENT_COLD)
							{
								if (m_IceBall_Skills[i].getUser() == NO_PLAYER)
								{
									IcaAgeBall iceBall{};
									m_IceBall_Skills[i] = iceBall;
									m_IceBall_Skills[i].setSkillType(SKILL_COLD1);
									m_IceBall_Skills[i].setUser(rEvent.playerID);
									m_IceBall_Skills[i].setPosition(rEvent.xmfPosition);
									m_IceBall_Skills[i].setRotate(rEvent.xmfRotate);
									m_IceBall_Skills[i].setTeam(m_players[j].getTeam());
									m_IceBall_Skills[i].set_playerSlotNum(m_players[j].getSlotNum());
									m_IceBall_Skills[i].MakeCollision();
									unsigned char Skilltype = m_IceBall_Skills[i].getSkillType();
									PushSkillCreate(i, Skilltype);
									break;
								}
							}
							else if (m_players[j].getCharacterType() == ELEMENT_DARKNESS)
							{
								if (m_Darkness_Enchantress_Skills[i].getUser() == NO_PLAYER)
								{
									Enchantress enchantress{};
									m_Darkness_Enchantress_Skills[i] = enchantress;
									m_Darkness_Enchantress_Skills[i].setSkillType(SKILL_DARKNESS1);
									m_Darkness_Enchantress_Skills[i].setUser(rEvent.playerID);
									m_Darkness_Enchantress_Skills[i].setPosition(rEvent.xmfPosition);
									m_Darkness_Enchantress_Skills[i].setRotate(rEvent.xmfRotate);
									m_Darkness_Enchantress_Skills[i].setTeam(m_players[j].getTeam());
									m_Darkness_Enchantress_Skills[i].set_playerSlotNum(m_players[j].getSlotNum());
									m_Darkness_Enchantress_Skills[i].MakeCollision();
									unsigned char Skilltype = m_Darkness_Enchantress_Skills[i].getSkillType();
									PushSkillCreate(i, Skilltype);
									break;
								}
							}
						}
						else if (rEvent.ucType1 == SKILL_E)
						{
							if (m_players[j].getCharacterType() == ELEMENT_FIRE)
							{
								if (m_FireMeteor_Skills[i].getUser() == NO_PLAYER)
								{
									Meteor meteor{};
									m_FireMeteor_Skills[i] = meteor;
									m_FireMeteor_Skills[i].setSkillType(SKILL_FIRE2);
									m_FireMeteor_Skills[i].setUser(rEvent.playerID);
									m_FireMeteor_Skills[i].setPosition(rEvent.xmfPosition);
									m_FireMeteor_Skills[i].setRotate(rEvent.xmfRotate);
									m_FireMeteor_Skills[i].setTeam(m_players[j].getTeam());
									m_FireMeteor_Skills[i].set_playerSlotNum(m_players[j].getSlotNum());
									m_FireMeteor_Skills[i].MakeCollision();
									unsigned char Skilltype = m_FireMeteor_Skills[i].getSkillType();

									PushSkillCreate(i, Skilltype);
									break;
								}
							}
							else if (m_players[j].getCharacterType() == ELEMENT_COLD)
							{
								if (m_IceFreeze_Skills[i].getUser() == NO_PLAYER)
								{
									IceFreeze ice_freeze{};
									m_IceFreeze_Skills[i] = ice_freeze;
									m_IceFreeze_Skills[i].setSkillType(SKILL_COLD2);
									m_IceFreeze_Skills[i].setUser(rEvent.playerID);
									m_IceFreeze_Skills[i].setPosition(rEvent.xmfPosition);
									m_IceFreeze_Skills[i].setRotate(rEvent.xmfRotate);
									m_IceFreeze_Skills[i].set_playerSlotNum(m_players[j].getSlotNum());
									m_IceFreeze_Skills[i].setTeam(m_players[j].getTeam());
									unsigned char Skilltype = m_IceFreeze_Skills[i].getSkillType();
									
									
									PushSkillCreate(i, Skilltype);
									break;
								}
							}
							else if (m_players[j].getCharacterType() == ELEMENT_DARKNESS)
							{
								if (m_Darkness_DistortionPearl_Skills[i].getUser() == NO_PLAYER)
								{
									DistortionPearl distortion_pearl{};
									m_Darkness_DistortionPearl_Skills[i] = distortion_pearl;
									m_Darkness_DistortionPearl_Skills[i].setSkillType(SKILL_DARKNESS2);
									m_Darkness_DistortionPearl_Skills[i].setUser(rEvent.playerID);
									m_Darkness_DistortionPearl_Skills[i].setPosition(rEvent.xmfPosition);
									m_Darkness_DistortionPearl_Skills[i].setRotate(rEvent.xmfRotate);
									m_Darkness_DistortionPearl_Skills[i].set_playerSlotNum(m_players[j].getSlotNum());
									m_Darkness_DistortionPearl_Skills[i].setTeam(m_players[j].getTeam());
									m_Darkness_DistortionPearl_Skills[i].MakeCollision();
									unsigned char Skilltype = m_Darkness_DistortionPearl_Skills[i].getSkillType();
									PushSkillCreate(i, Skilltype);
									break;
								}
							}
						}

					}
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
			//cout << "Error - Send_RoomInfo_Packet()\n";
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

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) { // 게임에 존재하는 모든 플레이어에게 메시지를 보내줘야한다.
			int id = m_players[i].getID();
			sendEvent_push(id, &packet);
		}
	}

}

void Room::PushIngame_PlayerInfo_Start(int id)
{
	if (this == nullptr) return;

	STOC_INGAME_STARTINFO packet;
	packet.size = sizeof(packet);
	packet.type = stoc_InGame_StartInfo;

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			packet.CharacterType = m_players[i].getCharacterType();
			packet.dwTeamNum = m_players[i].getTeam();
			packet.iHp = m_players[i].getHp();
			packet.xmfPosition = m_players[i].getPosition();
			packet.id = m_players[i].getID();
			sendEvent_push(id, &packet);
		}
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

void Room::Send_UpdatePlayerInfoPacket(Player& _player)
{
	if (this == nullptr) return;

	STOC_PlayerInfo packet;
	packet.size = sizeof(packet);
	packet.type = stoc_playerInfo;
	packet.ePlayerState = _player.getState();
	packet.Root_eAnimType = _player.getRootAnimType();
	packet.Upper_eAnimType = _player.getUpperAnimType();

	//packet.playerInfo.x = _player->getWorld();
	packet.playerInfo = _player.getInfo();
	packet.bAttackEnd = _player.IsAttackEnded();
	packet.playerInfo.iHp = _player.getHp();

	//m_player_mutex.lock();
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			//sendEvent_push(m_players[i].getID(), &packet);
			Server::GetInstance()->SendIngamePlayerInfo(m_players[i].getID(), packet);
		}
	}

	//m_player_mutex.unlock();

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

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
	}
}

void Room::PushBullet_Delete(int Bullet_Index)
{
	if (this == nullptr) return;

	STOC_Bullet_Delete packet;
	packet.size = sizeof(packet);
	packet.type = stoc_bullet_delete;
	packet.index = Bullet_Index;

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
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

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
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
		//recvEvnet_Clear();
		EVENT roomEvent_Send;
		roomEvent_Send.Object_ID = EVENT_KEY;
		roomEvent_Send.Target_ID = m_Info.Room_Num;
		roomEvent_Send.wakeup_time = chrono::system_clock::now() + chrono::seconds(1); // 1초에 한번
		roomEvent_Send.opType = OP_ROOM_SHOPPING_TIME;
		Server::GetInstance()->AddTimer(roomEvent_Send);


		unsigned char leftTime = m_TotalShoppingTime - m_ShoppingTime;
		//cout << "쇼핑시간 - " << (int)leftTime << "\n";
		++m_ShoppingTime;

		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) { // 게임중인 플레이어들에게 쇼핑시간 보내줘야지
				int id = m_players[i].getID();
				Server::GetInstance()->SendLeftTime(id, leftTime);
			}
		}


	}
	else
	{
		//cout << "라운드 시작\n";
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
		//cout << "라운드 진행중 - " << (int)leftTime << "\n";
		++m_RoundTime;

		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_players[i].getUsed()) { // 게임중인 플레이어들에게 쇼핑시간 보내줘야지
				int id = m_players[i].getID();
				Server::GetInstance()->SendLeftTime(id, leftTime);
			}
		}
	}
	else
	{
		m_isRoundEnd = true;
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
			
			PushRoundReset();
			m_isRoundReset = false;

		}
		else {
			
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

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
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
		int user = m_FireWall_Skills[slotNum].getUser();
		packet.user = user;
		int playerSlotNum = m_FireWall_Skills[slotNum].getplayerSlotNum();
		m_players[playerSlotNum].GetUpperFSM()->ChangeState(STATE_ATTACK, ANIM_SKILL_ATTACK);
		packet.xmfPosition = m_FireWall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_FireWall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_FIRE2)
	{
		int user = m_FireMeteor_Skills[slotNum].getUser();
		packet.user = user;
		int playerSlotNum = m_FireMeteor_Skills[slotNum].getplayerSlotNum();
		m_players[playerSlotNum].GetUpperFSM()->ChangeState(STATE_ATTACK, ANIM_SKILL_ATTACK);
		packet.xmfPosition = m_FireMeteor_Skills[slotNum].getPosition();
		packet.xmfRotate = m_FireMeteor_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_COLD1)
	{
		int user = m_IceBall_Skills[slotNum].getUser();
		packet.user = user;
		int playerSlotNum = m_IceBall_Skills[slotNum].getplayerSlotNum();
		m_players[playerSlotNum].GetUpperFSM()->ChangeState(STATE_ATTACK, ANIM_SKILL_ATTACK);
		packet.xmfPosition = m_IceBall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceBall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_COLD2)
	{
		int user = m_IceFreeze_Skills[slotNum].getUser();
		packet.user = user;
		int playerSlotNum = m_IceFreeze_Skills[slotNum].getplayerSlotNum();
		m_players[playerSlotNum].GetUpperFSM()->ChangeState(STATE_FREEZE, ANIM_IDLE);
		m_players[playerSlotNum].GetRootFSM()->ChangeState(STATE_FREEZE, ANIM_IDLE);
		packet.xmfPosition = m_IceFreeze_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceFreeze_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_DARKNESS1)
	{
		int user = m_Darkness_Enchantress_Skills[slotNum].getUser();
		packet.user = user;
		int playerSlotNum = m_Darkness_Enchantress_Skills[slotNum].getplayerSlotNum();
		m_players[playerSlotNum].GetUpperFSM()->ChangeState(STATE_ATTACK, ANIM_SKILL_ATTACK);
		packet.xmfPosition = m_Darkness_Enchantress_Skills[slotNum].getPosition();
		packet.xmfRotate = m_Darkness_Enchantress_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_DARKNESS2)
	{
		int user = m_Darkness_DistortionPearl_Skills[slotNum].getUser();
		packet.user = user;
		int playerSlotNum = m_Darkness_DistortionPearl_Skills[slotNum].getplayerSlotNum();
		m_players[playerSlotNum].GetUpperFSM()->ChangeState(STATE_ATTACK, ANIM_SKILL_ATTACK);
		packet.xmfPosition = m_Darkness_DistortionPearl_Skills[slotNum].getPosition();
		packet.xmfRotate = m_Darkness_DistortionPearl_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_CRESSVAS)
	{
		int user = m_IceCressvas_Skills[slotNum].getUser();
		packet.user = user;
		packet.xmfPosition = m_IceCressvas_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceCressvas_Skills[slotNum].getRotate();
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
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
		packet.user = m_FireMeteor_Skills[slotNum].getUser();
		packet.xmfPosition = m_FireMeteor_Skills[slotNum].getPosition();
		packet.xmfRotate = m_FireMeteor_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_COLD1)
	{
		packet.user = m_IceBall_Skills[slotNum].getUser();
		packet.xmfPosition = m_IceBall_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceBall_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_COLD2)
	{
		packet.user = m_IceFreeze_Skills[slotNum].getUser();
		packet.xmfPosition = m_IceFreeze_Skills[slotNum].getPosition();
		packet.xmfRotate = m_IceFreeze_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_DARKNESS1)
	{
		packet.user = m_Darkness_Enchantress_Skills[slotNum].getUser();
		packet.xmfPosition = m_Darkness_Enchantress_Skills[slotNum].getPosition();
		packet.xmfRotate = m_Darkness_Enchantress_Skills[slotNum].getRotate();
	}
	else if (SkillType == SKILL_DARKNESS2)
	{
		packet.user = m_Darkness_DistortionPearl_Skills[slotNum].getUser();
		packet.xmfPosition = m_Darkness_DistortionPearl_Skills[slotNum].getPosition();
		packet.xmfRotate = m_Darkness_DistortionPearl_Skills[slotNum].getRotate();
	}

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
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

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		if (m_players[i].getUsed()) {
			sendEvent_push(m_players[i].getID(), &packet);
		}
	}
}

Player* Room::GetPlayerForID(int id)
{
	for (auto& player : m_players)
		if (player.getID() == id)
			return &player;
	return nullptr;
}