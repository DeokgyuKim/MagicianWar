#include "include.h"
#include "StaticMeshMgr.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include <chrono>

#include "PlayerThread.h"
#include "Player.h"
#include "PlayerFSM.h"
#include "Bullet.h"

unordered_map<DWORD, bool> ReadyCheck;

unordered_map<DWORD, STOC_PlayerInfo> g_PlayerInfoPacket;
unordered_map<DWORD, DWORD> playerKeyInput;

extern unordered_map<int, STOC_PlayerInfo*>	g_Clients;
extern Player gClients[2];
extern int gClientNum; // 접속한 Client들의 명수
extern vector<PxRigidStatic*> StaticObjects;


mutex global_mutex;

list<Bullet> gBullets;
mutex gBullet_mutex;
bool gLateInit_LobbyScene = false;
bool gLateInit_MainScene = false;
bool gLateInit_EndingScene = false;
int iStaticObjectIdx = 0;

int gLiveTeamNum[2] = { 1, 1 };
float gEndingTime = 0;
bool gMaingameEnding = false;
float gTotalTime = 0;

int curScene = LOBBY_Scene; // 0: SCENE_LOGO 1: SCENE_LOBBY 2: SCENE_STAGE
chrono::system_clock::time_point prev_time;

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;		//recv()함수의 리턴 값 저장, 오류가 발생하지 않는다면 수신된 바이트 수를 리턴
	char* ptr = buf;	//buf의 시작 주소
	int left = len;		//수신해야할 데이터 크기, 아직 읽지 않은 데이터의 양

	while (left > 0)							//수신해야할 데이터가 남아있다면
	{
		received = recv(s, ptr, left, flags);	//recv()를 통해 데이터를 수신
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)					//연결 정상 종료
			break;
		left -= received;						//수신해야할 데이터 크기 - 현재 받은 데이터
		ptr += received;						//buf의 시작 주소 + 현재 받은 데이터
	}

	return (len - left);						//읽어야할 총 길이 - 수신해야할 데이터 양 = 읽은 데이터 양
}

void PlayerThread(STOC_ServerPlayer arg) // only recv & update data
{
	cout << "playerThread connect" << endl;
	auto player = arg;
	SOCKET clientSock = player.socket;
	DWORD playerID = player.info.dwPlayerNum;
	cout << playerID + 1 << "번 클라 접속" << endl;

	ReadyCheck[playerID] = false;
	if (playerID == 0) // 1번클라
		player.info.xmfPosition = XMFLOAT3(20.f, 0.f, 10.f);
	else
		player.info.xmfPosition = XMFLOAT3(30.f, 0.f, 90.f);

	gClients[playerID].setPosition(player.info.xmfPosition);

	//gClients[playerID].
	while (true)
	{
		int recv_Bytes = gClients[playerID].recvPacket();
		if (recv_Bytes == 0)
		{

		}
		else if (recv_Bytes == SOCKET_ERROR)
		{
			gClients[playerID].~Player();

			return;
		}
		else
		{
			char* buffer = gClients[playerID].getPacketStart_Ptr();
			short packet_size = (*reinterpret_cast<short*>(&buffer[0]));
			char* next_recv = gClients[playerID].getRecvStart_Ptr() + recv_Bytes;
			while (packet_size <= next_recv - buffer) // packet_size 클경우에만 정상크기의 패킷이 다 받아짐
			{
				packetProcessing(player); // recv 한 패킷들 처리
				buffer += packet_size;
				gClients[playerID].setPacketStart_Ptr(buffer);

				if (buffer < next_recv) { // 여러개 패킷이 동시에 넘어온 경우 다음 패킷 도 처리
					packet_size = (*reinterpret_cast<short*>(&buffer[0]));
				}
				else break;
			}



			// 버퍼의 공간이 없으면 데이터들을 다시 버퍼의 맨 앞으로 옮겨줌
			if (MAX_BUFFER - (next_recv - gClients[playerID].getBuffer()) < MIN_BUFFER)
			{
				int left_packet = next_recv - buffer;
				memcpy(gClients[playerID].getBuffer(), buffer, left_packet);
				gClients[playerID].resetPacket_Ptr();
				next_recv = gClients[playerID].getPacketStart_Ptr() + left_packet;
			}

			gClients[playerID].setRecvStart_Ptr(next_recv);
		}
	}
}

// recv 한 패킷들 처리
void packetProcessing(STOC_ServerPlayer arg)
{

	auto player = arg;
	SOCKET clientSock = player.socket;
	DWORD playerID = player.info.dwPlayerNum;

	// 패킷 처리
	char* processing = gClients[playerID].getPacketStart_Ptr();
	char packetType = processing[2]; // short(size) 0 1, (char)type


	switch (packetType)
	{
	case packet_keyInput: // 키 입력
	{
		CTOS_keyInput* data = (CTOS_keyInput*)processing;
		auto key = data->key;
		unsigned char keyID = data->id;
		gClients[data->id].setKeyInput(key);

		dynamic_cast<PlayerFSM*>(gClients[data->id].GetUpperFSM())->SetDefaultKey(key);
		dynamic_cast<PlayerFSM*>(gClients[data->id].GetRootFSM())->SetDefaultKey(key);

		//gClients[data->id].setKeyInput(key); // 키입력 업데이트 해주고

		break;
	}
	case packet_skill: // 스킬 발사
	{

		break;
	}
	case packet_ready: // Ready
	{
		CTOS_Ready* data = reinterpret_cast<CTOS_Ready*> (processing);
		gClients[data->id].setReady(data->ready); // 레디 갱신
		ReadyCheck[data->id] = gClients[data->id].getReady(); // 레디 갱신
		gClients[data->id].setCharacterType(data->CharacterType);

		bool Start = true;
		for (auto it = ReadyCheck.begin(); it != ReadyCheck.end(); ++it) { // 전체 순회
			if (it->second == false) { // 들어온 누구라도 false라면
				Start = false; // 스타트 안할거고
			}
		}


		if (Start && curScene == LOBBY_Scene) // 시작이고 현재 씬이 Lobby이면
		{
			gClients[0].setPlayerInitPos(XMFLOAT3(20.f, 0.f, 10.f));
			if (gClientNum > 1)
				gClients[1].setPlayerInitPos(XMFLOAT3(40.f, 0.f, 90.f));
			for (int i = 0; i < gClientNum; ++i)
			{
				gClients[i].CreateCapsuleController();
			}

			for (int i = 0; i < gClientNum; ++i) { // 전체 순회
				gClients[i].Lock();
				ReadyCheck[i] = false; // 한번만 타게 해주자 start를
				gClients[i].Unlock();
			}

			// 전체 클라에 한번씩 send하는 정보



			STOC_otherPlayerCount otherCount;
			otherCount.size = sizeof(otherCount);
			otherCount.type = stoc_otherPlayerNum;
			otherCount.playerCount = gClientNum - 1; // 나 자신을 뺀 인원수


			gClients[player.info.dwPlayerNum].Lock();
			gClients[player.info.dwPlayerNum].setPlayerInfo(player.info);
			gClients[player.info.dwPlayerNum].Unlock();

			for (int j = 0; j < gClientNum; ++j) {
				STOC_startInfo sI;
				sI.size = sizeof(sI);
				sI.type = stoc_startInfo;
				sI.dwPlayerNum = gClients[j].getInfo().info.dwPlayerNum;
				sI.dwTeamNum = gClients[j].getInfo().info.dwTeamNum;
				sI.xmfPosition = gClients[j].getInfo().info.xmfPosition;
				sI.CharacterType = gClients[j].getCharacterType();
				sI.iHp = 100;

				gClients[j].sendPacket((void*)&sI, sI.size); // 나 자신의 정보				
				gClients[j].sendPacket((void*)&otherCount, otherCount.size); // 나 자신을 뺀 인원 수
				for (int i = 0; i < gClientNum; ++i) {
					if (gClients[i].getInfo().info.dwPlayerNum != gClients[j].getInfo().info.dwPlayerNum) {
						// 나 자신을 제외한 애들의 정보 send
						STOC_OtherstartInfo otherPlayerInfo;
						otherPlayerInfo.size = sizeof(otherPlayerInfo);
						otherPlayerInfo.type = stoc_OtherstartInfo;
						otherPlayerInfo.CharacterType = gClients[i].getCharacterType();
						otherPlayerInfo.dwPlayerNum = gClients[i].getInfo().info.dwPlayerNum;
						otherPlayerInfo.dwTeamNum = gClients[i].getInfo().info.dwTeamNum;
						otherPlayerInfo.xmfPosition = gClients[i].getInfo().info.xmfPosition;
						otherPlayerInfo.iHp = 100;

						gClients[j].sendPacket((void*)&otherPlayerInfo, otherPlayerInfo.size);
					}
				}
			}
			curScene = STAGE_Scene; // 씬 전환 -> Stage
			gLateInit_MainScene = false;

		}
		break;
	}
	case ctos_playerInfo: // clinet에서 보낸 playerInfo
	{
		CTOS_PlayerInfo* data = reinterpret_cast<CTOS_PlayerInfo*> (processing);
		//cout << "Root_Blend Type - " << (int)data->Root_eAnimBlendType << endl;
		//cout << "Root_CurAni Type - " << (int)data->Root_eAnimType << endl;
		//cout << "Upper_Blend Type - " << (int)data->Upper_eAnimBlendType << endl;
		//cout << "Upper_CurAni Type - " << (int)data->Upper_eAnimType << endl;

		gClients[data->id].UpdatePlayerInfo(data);
		break;
	}

	default:
		cout << "NoType Packet" << endl;
		break;
	}
}

void WorkThread() // send & physics & function
{
	cout << "workThread connect" << endl;
	while (true)
	{
		if (curScene == LOADING_Scene) // Logo
		{

		}
		else if (curScene == LOBBY_Scene) // Lobby
		{
			if (!gLateInit_LobbyScene) {
				cout << "여긴 로비" << endl;
				gLateInit_LobbyScene = true;
			}
		}
		else if (curScene == STAGE_Scene) // Stage
		{
			if (!gLateInit_MainScene) {
				cout << "여긴 스테이지" << endl;
				STOC_sceneChange sc;
				sc.size = sizeof(sc);
				sc.type = stoc_sceneChange;
				sc.sceneNum = STAGE_Scene; //
				for (int i = 0; i < gClientNum; ++i)
					gClients[i].sendPacket((void*)&sc, sc.size); // stoc_sceneChange
				gEndingTime = 0;
				gMaingameEnding = false;
				gTotalTime = 0;
				for (int i = 0; i < gClientNum; ++i)
				{
					gClients[i].GetUpperFSM()->ChangeState(static_cast<int>(PLAYER_STATE::IDLE), static_cast<int>(ANIMATION_TYPE::IDLE));
					gClients[i].GetRootFSM()->ChangeState(static_cast<int>(PLAYER_STATE::IDLE), static_cast<int>(ANIMATION_TYPE::IDLE));
				}

				gLateInit_MainScene = true;
			}

			auto start_time = chrono::system_clock::now();
			chrono::duration<float> frame_time = start_time - prev_time;

			STOC_GameEnd gameEnd;
			gameEnd.size = sizeof(gameEnd);
			gameEnd.type = stoc_gameend;
			gameEnd.bEnd = false;
			gameEnd.teamNum = 0;
			//cout << "Time: " << frame_time.count() << endl;
			gTotalTime += frame_time.count();

			//if (gTotalTime >= 2.f) {
			//	gClients[0].setPlayerHp(gClients[0].getInfo().info.iHp - 10.f);
			//	if (gClients[0].getInfo().info.iHp <= 0) { // 0번팀 패배 시뮬레이션
			//		gClients[0].GetUpperFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DEAD), static_cast<int>(ANIMATION_TYPE::DEAD));
			//		gClients[0].GetRootFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DEAD), static_cast<int>(ANIMATION_TYPE::DEAD));
			//		gameEnd.bEnd = true;
			//		gMaingameEnding = true;
			//		gameEnd.teamNum = 1;
			//		}
			//	else {
			//		gClients[0].GetUpperFSM()->ChangeState(static_cast<int>(PLAYER_STATE::HIT), static_cast<int>(ANIMATION_TYPE::HIT));					
			//	}
			//	gTotalTime = 0.f;
			//}
			DWORD key[2];
			unsigned char User[2];
			unsigned char InstanceName[2];
			Bullet temp{};
			for (int i = 0; i < gClientNum; ++i)
			{
				gClients[i].Lock();
				key[i] = gClients[i].getKey();
				User[i] = gClients[i].getID();
				InstanceName[i] = gClients[i].getInstanceName();

				if (gClients[i].IsConnected()) // 연결 됐다면
				{
					gClients[i].Update(frame_time.count()); // 플레이어들 Update 키입력에 따른 위치 변화
					//if (key[i] & 0x0020) {

						//cout << "총알 갯수" << gClients[i].getCreateBullet() << endl;
						//if(key[i] & 0x)
					if (gClients[i].getCreateBullet() == 1) { // 왼쪽 클릭

						temp.SetUser(User[i]);
						temp.setInstanceName(InstanceName[i]);
						temp.setScale(XMFLOAT3{ 0.3f,0.3f,0.3f });
						temp.setRotate(XMFLOAT3{ 0.f,0.f,0.f });
						temp.setWorldMatrix(gClients[i].getBulletStartWorld());
						temp.setPosition(XMFLOAT3{ gClients[i].getWorld()._41,gClients[i].getWorld()._42 + 1.f,gClients[i].getWorld()._43 });
						temp.setTotalLifeTime(5.f);
						temp.setDirection(XMFLOAT3(-gClients[i].getWorld()._21, -gClients[i].getWorld()._22, -gClients[i].getWorld()._23));

						if (gBullets.size() < BulletCB_Count)
							gBullets.push_back(temp); // list에 담아
						gClients[i].setCreateBullet(0);

					}
					// 충돌체크?
					Physics_Collision(i);

				}
				gClients[i].Unlock();
			}

			//gBullet_mutex.lock();
			auto iter = gBullets.begin();
			while (iter != gBullets.end())
			{ // 총알 업데이트
				int dead;


				dead = iter->Update(frame_time.count()); // TotalTime < lifeTime 삭제 (Dead)
				iter->LateUpdate(frame_time.count());

				if (dead) {

					iter = gBullets.erase(iter);
				}
				else {

					iter++;
				}


			}

			int BulletsConut = gBullets.size();
			//Bullet_Packet* bullet_Packet = nullptr;
			STOC_Bullet InstBullets;
			InstBullets.size = sizeof(InstBullets);
			InstBullets.type = stoc_bullet;
			InstBullets.Bullet_Count = 0;
			InstBullets.bullets[0].id = -1;
			if (BulletsConut != 0)
			{ // 총알이 있으면

				auto iter_packet = gBullets.begin();
				int b_count = 0;
				while (iter_packet != gBullets.end())
				{
					InstBullets.bullets[b_count].id = iter_packet->getUser();
					InstBullets.bullets[b_count].InstanceName = iter_packet->getInstanceName();
					InstBullets.bullets[b_count].lifeTime = iter_packet->getLifeTime();
					InstBullets.bullets[b_count].matWorld = iter_packet->getWorld();
					++iter_packet;
					++b_count;
				}
				//InstBullets.bullets = bullet_Packet;
				InstBullets.Bullet_Count = gBullets.size();
			}


			//Collision
			for (int i = 0; i < gClientNum; ++i)
			{
				//플레이어가 히또 상태면 체크 안하게
				if (!(gClients[i].GetUpperFSM()->GetState() == PLAYER_STATE::HIT && gClients[i].GetUpperFSM()->GetState() == PLAYER_STATE::DANCE && gClients[i].GetUpperFSM()->GetState() == PLAYER_STATE::DEAD))
				{
					for (auto iter = gBullets.begin(); iter != gBullets.end();)
					{
						if (gClients[i].getInfo().info.dwPlayerNum == (DWORD)iter->getUser())
						{
							++iter;
							continue;
						}
						if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(gClients[i].GetPxCapsuleController()->getActor(), iter->GetRigidDynamic()))
						{
							int iDamage = 0;
							switch ((*iter).getInstanceName())
							{
							case WIZARD_FIRE:
								iDamage = 10;
							}
							gClients[i].setPlayerHp(gClients[i].getInfo().info.iHp - iDamage);
							if (gClients[i].getInfo().info.iHp <= 0)
							{
								//플레이어 SetFSM(DEAD)
								///////////////////////
								gClients[i].GetUpperFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DEAD), static_cast<int>(ANIMATION_TYPE::DEAD));
								gClients[i].GetRootFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DEAD), static_cast<int>(ANIMATION_TYPE::DEAD));
								--gLiveTeamNum[gClients[i].getInfo().info.dwTeamNum];
								if (gLiveTeamNum[gClients[i].getInfo().info.dwTeamNum] <= 0)
								{

									gameEnd.bEnd = true;
									gMaingameEnding = true;
									////해당팀이 진거
									/////////////////
									if (gClients[i].getInfo().info.dwTeamNum == 0) {
										// 0 번팀 패배
										gClients[1].GetUpperFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DANCE), static_cast<int>(ANIMATION_TYPE::DANCE));
										gClients[1].GetRootFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DANCE), static_cast<int>(ANIMATION_TYPE::DANCE));
										gameEnd.teamNum = 1;
									}
									else {
										// 1 번팀 패배
										gClients[0].GetUpperFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DANCE), static_cast<int>(ANIMATION_TYPE::DANCE));
										gClients[0].GetRootFSM()->ChangeState(static_cast<int>(PLAYER_STATE::DANCE), static_cast<int>(ANIMATION_TYPE::DANCE));
										gameEnd.teamNum = 0;
									}

								}
							}
							else
								gClients[i].GetUpperFSM()->ChangeState((int)PLAYER_STATE::HIT, (int)ANIMATION_TYPE::HIT);
							iter = gBullets.erase(iter);
						}
						else
							++iter;
					}
				}
			}

			//for (auto staticMesh : StaticObjects)
			//{

#ifdef _DEBUG
			for (int i = 0; i < 20; ++i)
			{
				if (iStaticObjectIdx + i >= StaticObjects.size())
					break;
				for (auto iter = gBullets.begin(); iter != gBullets.end();)
				{
					if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject((*iter).GetRigidDynamic(), StaticObjects[iStaticObjectIdx + i]))
					{
						iter = gBullets.erase(iter);
					}
					else
						++iter;
				}
			}
			iStaticObjectIdx = (iStaticObjectIdx + 20);
			if (iStaticObjectIdx >= StaticObjects.size())
				iStaticObjectIdx -= StaticObjects.size();
#else
			for (auto object : StaticObjects)
			{
				for (auto iter = gBullets.begin(); iter != gBullets.end();)
				{
					if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject((*iter).GetRigidDynamic(), object))
					{
						iter = gBullets.erase(iter);
					}
					else
						++iter;
				}
			}
#endif
			//}
			//gBullet_mutex.unlock();

			///PhysXUpdate
			CPhysXMgr::GetInstance()->gScene->simulate(frame_time.count());
			CPhysXMgr::GetInstance()->gScene->fetchResults(true);



			for (int i = 0; i < gClientNum; ++i)
			{
				gClients[i].Lock();
				if (gClients[i].IsConnected()) // 연결 됐다면
				{
					gClients[i].ModifyPhysXPos(frame_time.count());
					UpdatePlayerInfoPacket(gClients[i].getInfo().info.dwPlayerNum, gClients[i]);
				}
				gClients[i].Unlock();
			}

			InstBullets.size = sizeof(InstBullets) - (sizeof(Bullet_Packet) * (BulletCB_Count - InstBullets.Bullet_Count));
			if (gMaingameEnding) {
				gEndingTime += frame_time.count();
			}
			// send

			for (int i = 0; i < gClientNum; ++i) {
				//gClients[i].Lock();
				for (auto it = g_PlayerInfoPacket.begin(); it != g_PlayerInfoPacket.end(); ++it) {
					//cout << it->second.playerInfo.dwPlayerNum << " - ( " << it->second.matWorld._41 << ", " << it->second.matWorld._42 << ", " << it->second.matWorld._43 << " )" << endl;
					gClients[i].sendPacket((void*)&it->second, sizeof(it->second)); // 모든 정보
				}

				gClients[i].sendPacket((void*)&InstBullets, InstBullets.size);
				gClients[i].sendPacket((void*)&gameEnd, sizeof(gameEnd));

				//gClients[i].Unlock();

			}
			if (gEndingTime >= 5.f) { // 춤 5초 추고 엔딩으로
				STOC_sceneChange scenePacket;
				scenePacket.size = sizeof(scenePacket);
				scenePacket.type = stoc_sceneChange;
				scenePacket.sceneNum = LOBBY_Scene; // 엔딩 씬
				STOC_GameEnd gameEnd;
				gameEnd.size = sizeof(gameEnd);
				gameEnd.type = stoc_gameend;
				gameEnd.bEnd = false;
				gameEnd.teamNum = -1;
				for (int i = 0; i < gClientNum; ++i) {
					gClients[i].ReInit();
					gClients[i].sendPacket((void*)&scenePacket, scenePacket.size);
					gClients[i].sendPacket((void*)&gameEnd, gameEnd.size);
				}
				global_mutex.lock();
				curScene = LOBBY_Scene;
				gLateInit_LobbyScene = false;
				gMaingameEnding = false;
				global_mutex.unlock();
			}

			// 다 보낸 후에는 workThread를 일시정지

			auto end_time = chrono::system_clock::now();
			prev_time = end_time;
			auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
#ifdef _DEBUG
			this_thread::sleep_for(12ms - elapsed_time);
#else
			this_thread::sleep_for(8ms - elapsed_time);
#endif // _DEBUG

		}
		else if (curScene == ENDING_Scene) { // 엔딩 씬
			auto start_time = chrono::system_clock::now();
			chrono::duration<float> frame_time = start_time - prev_time;

			//if (!gLateInit_EndingScene)
			//{
			//	cout << "여긴 엔딩" << endl;
			//	gEndingTime = 0;
			//	gLateInit_EndingScene = true;
			//}

			//gEndingTime += frame_time.count();


			if (gEndingTime >= 1.f) { // 엔딩씬 이후 5초가 지나면 Lobby로
				global_mutex.lock();
				gLateInit_LobbyScene = false;
				global_mutex.unlock();
				STOC_sceneChange scenePacket;
				scenePacket.size = sizeof(scenePacket);
				scenePacket.type = stoc_sceneChange;
				scenePacket.sceneNum = LOBBY_Scene; // 로비 씬
				STOC_GameEnd gameEnd;
				gameEnd.size = sizeof(gameEnd);
				gameEnd.type = stoc_gameend;
				gameEnd.bEnd = false;
				gameEnd.teamNum = -1;
				for (int i = 0; i < gClientNum; ++i) {
					gClients[i].ReInit();
					gClients[i].sendPacket((void*)&scenePacket, scenePacket.size); // 로비씬으로 넘어가라
					gClients[i].sendPacket((void*)&gameEnd, gameEnd.size);
				}
				gEndingTime = 0;
				curScene = LOBBY_Scene;
			}

			auto end_time = chrono::system_clock::now();
			prev_time = end_time;
			auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
			this_thread::sleep_for(12ms - elapsed_time);

		}
	}
}


void Physics_Collision(int id)
{
	// 
}

void UpdatePlayerInfoPacket(int id, Player& _player)
{
	g_PlayerInfoPacket[id].size = sizeof(g_PlayerInfoPacket[id]);
	g_PlayerInfoPacket[id].type = stoc_playerInfo;
	g_PlayerInfoPacket[id].ePlayerState = _player.getState();

	g_PlayerInfoPacket[id].Root_eAnimType = _player.getRootAnimType();
	g_PlayerInfoPacket[id].Upper_eAnimType = _player.getUpperAnimType();

	g_PlayerInfoPacket[id].matWorld = _player.getWorld();
	g_PlayerInfoPacket[id].playerInfo = _player.getInfo().info;
	g_PlayerInfoPacket[id].bAttackEnd = _player.IsAttackEnded();
	g_PlayerInfoPacket[id].playerInfo.iHp = _player.getHp();
	//g_PlayerInfoPacket[id].Root_eAnimBlendType = _player.getRootAnimBlendType();
	//g_PlayerInfoPacket[id].Root_fAnimTime = _player.getRootAnimTime();
	//g_PlayerInfoPacket[id].Root_fWeight = _player.getRootAnimWeight();

	//g_PlayerInfoPacket[id].Upper_eAnimBlendType = _player.getUpperAnimBlendType();
	//g_PlayerInfoPacket[id].Upper_fAnimTime = _player.getUpperAnimTime();
	//g_PlayerInfoPacket[id].Upper_fWeight = _player.getUpperAnimWeight();
	//
}

