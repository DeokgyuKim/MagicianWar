#include "include.h"
#include "StaticMeshMgr.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <chrono>

#include "PlayerThread.h"
#include "Player.h"
#include "PlayerFSM.h"

unordered_map<DWORD, bool> ReadyCheck;
unordered_map<DWORD, STOC_PlayerInfo> g_PlayerInfoPacket;
unordered_map<DWORD, DWORD> playerKeyInput;
extern unordered_map<int, STOC_PlayerInfo*>	g_Clients;
extern Player gClients[2];
extern int gClientNum; // 접속한 Client들의 명수

unsigned int curScene = 1; // 1: SCENE_LOBBY 2: SCENE_
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
	cout << playerID+1 << "번 클라 접속" << endl;

	ReadyCheck[playerID] = false;
	if (playerID == 0) // 1번클라
		player.info.xmfPosition = XMFLOAT3(20.f, 0.f, 10.f);
	else
		player.info.xmfPosition = XMFLOAT3(20.f, 0.f, 15.f);

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
	bool sendOneTime = false;
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
		gClients[data->id].Lock();
		ReadyCheck[data->id] = gClients[data->id].getReady(); // 레디 갱신
		gClients[data->id].Unlock();
		bool Start = true;
		for (auto it = ReadyCheck.begin(); it != ReadyCheck.end(); ++it) { // 전체 순회
			if (it->second == false) { // 들어온 누구라도 false라면
				Start = false; // 스타트 안할거고
			}
		}
		if (Start && curScene == 1) // 시작이고 현재 씬이 Lobby이면
		{
			gClients[0].setPlayerInitPos(XMFLOAT3(20.f, 0.f, 10.f));
			//gClients[1].setPlayerInitPos(XMFLOAT3(20.f, 0.f, 15.f));
			for (int i = 0; i < gClientNum; ++i)
			{
				gClients[i].CreateCapsuleController();
			}

			for (int i = 0; i < gClientNum; ++i) { // 전체 순회
				ReadyCheck[i] = false; // 한번만 타게 해주자 start를
			}
			// 전체 클라에 한번씩 send하는 정보
			STOC_sceneChange sc;
			sc.size = sizeof(sc);
			sc.type = stoc_sceneChange;
			sc.sceneNum = 2; // 이게 메인scene넘버



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
				
				gClients[j].sendPacket((void*)&sc, sc.size); // stoc_sceneChange
				gClients[j].sendPacket((void*)&sI, sI.size); // 나 자신의 정보				
				gClients[j].sendPacket((void*)&otherCount, otherCount.size); // 나 자신을 뺀 인원 수
				for (int i = 0; i < gClientNum; ++i) {
					if (gClients[i].getInfo().info.dwPlayerNum != gClients[j].getInfo().info.dwPlayerNum) {
						// 나 자신을 제외한 애들의 정보 send
						STOC_OtherstartInfo otherPlayerInfo;
						otherPlayerInfo.size = sizeof(otherPlayerInfo);
						otherPlayerInfo.type = stoc_OtherstartInfo;
						otherPlayerInfo.dwPlayerNum = gClients[i].getInfo().info.dwPlayerNum;
						otherPlayerInfo.dwTeamNum = gClients[i].getInfo().info.dwTeamNum;
						otherPlayerInfo.xmfPosition = gClients[i].getInfo().info.xmfPosition;

						gClients[j].sendPacket((void*)&otherPlayerInfo, otherPlayerInfo.size);
					}
				}
			}
			curScene = 2; // 씬 전환 -> Stage
			sendOneTime = true;
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
		if (curScene == 1) // Lobby
		{

		}
		else if (curScene == 2) // Stage
		{
			auto start_time = chrono::system_clock::now();
			chrono::duration<float> frame_time = start_time - prev_time;
			for (int i = 0; i < gClientNum; ++i)
			{
				gClients[i].Lock();
				if (gClients[i].IsConnected()) // 연결 됐다면
				{
					gClients[i].Update(); // 플레이어들 Update 키입력에 따른 위치 변화
					// 충돌체크?
					Physics_Collision(i);

				}
				gClients[i].Unlock();
			}
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
			// send

			for (int i = 0; i < gClientNum; ++i) {
				for (auto it = g_PlayerInfoPacket.begin(); it != g_PlayerInfoPacket.end(); ++it) {
					//cout << it->second.playerInfo.dwPlayerNum << " - ( " << it->second.matWorld._41 << ", " << it->second.matWorld._42 << ", " << it->second.matWorld._43 << " )" << endl;
					gClients[i].sendPacket((void*)&it->second, sizeof(it->second)); // 모든 정보


				}

			}
			// 다 보낸 후에는 workThread를 일시정지
			auto end_time = chrono::system_clock::now();
			prev_time = end_time;
			auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

			this_thread::sleep_for(8ms - elapsed_time);
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
	//g_PlayerInfoPacket[id].Root_eAnimBlendType = _player.getRootAnimBlendType();
	//g_PlayerInfoPacket[id].Root_fAnimTime = _player.getRootAnimTime();
	//g_PlayerInfoPacket[id].Root_fWeight = _player.getRootAnimWeight();

	//g_PlayerInfoPacket[id].Upper_eAnimBlendType = _player.getUpperAnimBlendType();
	//g_PlayerInfoPacket[id].Upper_fAnimTime = _player.getUpperAnimTime();
	//g_PlayerInfoPacket[id].Upper_fWeight = _player.getUpperAnimWeight();
	//
}

