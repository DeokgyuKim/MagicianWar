#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <winsock2.h>
#include <chrono>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#include "d3dUtil.h"


using namespace DirectX;
using namespace DirectX::PackedVector;

#include "PlayerThread.h"
#include "Player.h"


unordered_map<DWORD, bool> ReadyCheck;
unordered_map<DWORD, STOC_PlayerInfo> g_PlayerInfoPacket;
unordered_map<DWORD, DWORD> playerKeyInput;
extern unordered_map<int, STOC_PlayerInfo*>	g_Clients;
extern Player gClients[2];
extern int gClientNum; // ������ Client���� ���

unsigned int curScene = 1; // 1: SCENE_LOBBY 2: SCENE_
chrono::system_clock::time_point prev_time;

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;		//recv()�Լ��� ���� �� ����, ������ �߻����� �ʴ´ٸ� ���ŵ� ����Ʈ ���� ����
	char* ptr = buf;	//buf�� ���� �ּ�
	int left = len;		//�����ؾ��� ������ ũ��, ���� ���� ���� �������� ��

	while (left > 0)							//�����ؾ��� �����Ͱ� �����ִٸ�
	{
		received = recv(s, ptr, left, flags);	//recv()�� ���� �����͸� ����
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)					//���� ���� ����
			break;
		left -= received;						//�����ؾ��� ������ ũ�� - ���� ���� ������
		ptr += received;						//buf�� ���� �ּ� + ���� ���� ������
	}

	return (len - left);						//�о���� �� ���� - �����ؾ��� ������ �� = ���� ������ ��
}

void PlayerThread(STOC_ServerPlayer arg) // only recv & update data
{
	cout << "playerThread connect" << endl;
	auto player = arg;
	SOCKET clientSock = player.socket;
	DWORD playerID = player.info.dwPlayerNum;
	cout << playerID << "�� Ŭ�� ����" << endl;

	ReadyCheck[playerID] = false;
	player.info.xmfPosition = XMFLOAT3(20.f, 0.f, 10.f);
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
			while (packet_size <= next_recv - buffer) // packet_size Ŭ��쿡�� ����ũ���� ��Ŷ�� �� �޾���
			{
				packetProcessing(player); // recv �� ��Ŷ�� ó��
				buffer += packet_size;
				gClients[playerID].setPacketStart_Ptr(buffer);

				if (buffer < next_recv) { // ������ ��Ŷ�� ���ÿ� �Ѿ�� ��� ���� ��Ŷ �� ó��
					packet_size = (*reinterpret_cast<short*>(&buffer[0]));
				}
				else break;
			}



			// ������ ������ ������ �����͵��� �ٽ� ������ �� ������ �Ű���
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

// recv �� ��Ŷ�� ó��
void packetProcessing(STOC_ServerPlayer arg)
{
	bool sendOneTime = false;
	auto player = arg;
	SOCKET clientSock = player.socket;
	DWORD playerID = player.info.dwPlayerNum;

	// ��Ŷ ó��
	char* processing = gClients[playerID].getPacketStart_Ptr();
	char packetType = processing[2]; // short(size) 0 1, (char)type


	switch (packetType)
	{
	case packet_keyInput: // Ű �Է�
	{
		CTOS_keyInput* data = (CTOS_keyInput*)processing;
		auto key = data->key;
		gClients[data->id].setKeyInput(key); // Ű�Է� ������Ʈ ���ְ�

		break;
	}
	case packet_skill: // ��ų �߻�
	{

		break;
	}
	case packet_ready: // Ready
	{
		CTOS_Ready* data = reinterpret_cast<CTOS_Ready*> (processing);
		gClients[data->id].setReady(data->ready); // ���� ����
		gClients[data->id].Lock();
		ReadyCheck[data->id] = gClients[data->id].getReady(); // ���� ����
		gClients[data->id].Unlock();
		bool Start = true;
		for (auto it = ReadyCheck.begin(); it != ReadyCheck.end(); ++it) { // ��ü ��ȸ
			if (it->second == false) { // ���� ������ false���
				Start = false; // ��ŸƮ ���ҰŰ�
			}
		}
		if (Start && curScene == 1) // �����̰� ���� ���� Lobby�̸�
		{
			for (int i = 0; i < gClientNum; ++i) { // ��ü ��ȸ
				ReadyCheck[i] = false; // �ѹ��� Ÿ�� ������ start��
			}
			// ��ü Ŭ�� �ѹ��� send�ϴ� ����
			STOC_sceneChange sc;
			sc.size = sizeof(sc);
			sc.type = stoc_sceneChange;
			sc.sceneNum = 2; // �̰� ����scene�ѹ�

			STOC_startInfo sI;
			sI.size = sizeof(sI);
			sI.type = stoc_startInfo;
			sI.dwPlayerNum = player.info.dwPlayerNum;
			sI.dwTeamNum = player.info.dwTeamNum;
			sI.xmfPosition = player.info.xmfPosition;

			STOC_otherPlayerCount otherCount;
			otherCount.size = sizeof(otherCount);
			otherCount.type = stoc_otherPlayerNum;
			otherCount.playerCount = gClientNum - 1; // �� �ڽ��� �� �ο���


			gClients[player.info.dwPlayerNum].Lock();
			gClients[player.info.dwPlayerNum].setPlayerInfo(player.info);
			gClients[player.info.dwPlayerNum].Unlock();

			for (int j = 0; j < gClientNum; ++j) {
				gClients[j].sendPacket((void*)&sc, sc.size); // stoc_sceneChange
				//gClients[j].sendPacket((void*)&sI, sI.size); // �� �ڽ��� ����
				gClients[j].sendPacket((void*)&otherCount, otherCount.size); // �� �ڽ��� �� �ο� ��
				for (int i = 0; i < gClientNum; ++i) {
					if (gClients[i].getInfo().info.dwPlayerNum != gClients[j].getInfo().info.dwPlayerNum) {
						// �� �ڽ��� ������ �ֵ��� ���� send
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
			curScene = 2; // �� ��ȯ -> Stage
			sendOneTime = true;
		}
		break;
	}
	case ctos_playerInfo: // clinet���� ���� playerInfo
	{
		CTOS_PlayerInfo* data = reinterpret_cast<CTOS_PlayerInfo*> (processing);
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
			chrono::duration<double> frame_time = start_time - prev_time;
			for (int i = 0; i < gClientNum; ++i)
			{
				gClients[i].Lock();
				if (gClients[i].IsConnected()) // ���� �ƴٸ�
				{
					gClients[i].Update(); // �÷��̾�� Update Ű�Է¿� ���� ��ġ ��ȭ
					// �浹üũ?
					Physics_Collision(i);
					UpdatePlayerInfoPacket(gClients[i].getInfo().info.dwPlayerNum, gClients[i]);

				}
				gClients[i].Unlock();
			}
			// send
			
			for (int i = 0; i < gClientNum; ++i) {
				for (auto it = g_PlayerInfoPacket.begin(); it != g_PlayerInfoPacket.end(); ++it) {
					//cout << it->second.playerInfo.dwPlayerNum << " - ( " << it->second.matWorld._41 << ", " << it->second.matWorld._42 << ", " << it->second.matWorld._43 << " )" << endl;
					gClients[i].sendPacket((void*)&it->second, sizeof(it->second)); // ��� ����
					
					
				}

			}
			// �� ���� �Ŀ��� workThread�� �Ͻ�����
			auto end_time = chrono::system_clock::now();
			prev_time = end_time;
			auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

			this_thread::sleep_for(16ms - elapsed_time);
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
	g_PlayerInfoPacket[id].eAnimBlendType = _player.getAnimBlendType();
	g_PlayerInfoPacket[id].ePlayerState = _player.getState();
	g_PlayerInfoPacket[id].eAnimType = _player.getAnimType();
	g_PlayerInfoPacket[id].fAnimTime = _player.getAnimTime();
	g_PlayerInfoPacket[id].fWeight = _player.getAnimWeight();
	g_PlayerInfoPacket[id].matWorld = _player.getWorld();
	g_PlayerInfoPacket[id].playerInfo = _player.getInfo().info;
}


//DWORD WINAPI PlayerThread(LPVOID arg)
//{
//	while (true) // loop
//	{
//
//		//while (true)
//		//{
//		//	DWORD buf;
//		//	// ������¸� ��� �޾������
//		//	recvn(client_sock, (char*)&buf, sizeof(buf), 0);
//		//	if (buf == 1) { std::cout << "Ready �Ϸ�" << endl; }
//		//	//else cout << "���� ����" << endl;
//		//	std::cout << buf << endl;
//
//		//	if (buf == 1)
//		//		ReadyCheck[player->info.dwPlayerNum] = true; // �ش� �÷��̾�� ����
//		//	else
//		//		ReadyCheck[player->info.dwPlayerNum] = false; // �ش� �÷��̾�� ���� ����
//
//		//	bool Ready = true;
//		//	int PlayerCount = 0;
//		//	for (auto it = ReadyCheck.begin(); it != ReadyCheck.end(); ++it) { // ��ü ��ȸ
//		//		if (it->second == false) { // ���� ������ false���
//		//			Ready = false; // ��ŸƮ ���ҰŰ�
//		//		}
//		//		++PlayerCount;
//		//	}
//
//
//		//	send(client_sock, (char*)&Ready, sizeof(Ready), 0); // ���� üũ�� ���� �����ְ�
//
//		//	if (Ready) { // ��ȸ�ؼ� �� ������
//		//		--PlayerCount; // �� �ڽ��� ��
//		//		myInfo.xmfPosition = XMFLOAT3(20.f, 0.f, 10.f);
//		//		send(client_sock, (char*)&myInfo, sizeof(myInfo), 0); // �� �ڽ��� ����
//		//		send(client_sock, (char*)&PlayerCount, sizeof(PlayerCount), 0); // �ο��� ������
//		//		for (auto it = g_Clients.begin(); it != g_Clients.end(); ++it) {
//
//		//			if (it->second->info.dwPlayerNum != player->info.dwPlayerNum) { // �� �ڽ� ���ܶ��
//		//				PlayerInfo _pinfo;
//		//				_pinfo = it->second->info;
//		//				send(client_sock, (char*)&_pinfo, sizeof(_pinfo), 0); // �� �ڽ� ������ �ֵ��� playerinfo �Ѱ���
//		//			}
//		//		}
//		//		curScene = 2; // ���� ��
//		//		break;
//		//	}
//		//}
//
//		while (true) {
//
//			//CTOS_PlayerInfo _pInfo;
//			//DWORD _keyInput;
//			//recvn(client_sock, (char*)&_pInfo, sizeof(_pInfo), 0);
//			//recvn(client_sock, (char*)&_keyInput, sizeof(_keyInput), 0);
//			//
//			//UpdatePlayerInfo(_pInfo, player->info.dwPlayerNum); // player ��ġ �޾��ֱ�
//			//playerKeyInput[player->info.dwPlayerNum] = _keyInput; // player Ű�Է� �޾��ֱ�
//
//			UpdatePosition(player->info.dwPlayerNum);
//
//
//			for (auto it = g_MapRecvPlayerInfo.begin(); it != g_MapRecvPlayerInfo.end(); ++it) {
//				send(client_sock, (char*)it->second, sizeof(*it->second), 0); // ��� ����
//			}
//			//cout << "Ű - " << playerKeyInput[player->info.dwPlayerNum]<<"\n";
//			//cout << "x - " << g_MapRecvPlayerInfo[player->info.dwPlayerNum]->matWorld._41 << ", " <<
//			//	"y - " << g_MapRecvPlayerInfo[player->info.dwPlayerNum]->matWorld._42 << ", " <<
//			//	"z - " << g_MapRecvPlayerInfo[player->info.dwPlayerNum]->matWorld._43 << "\n";
//
//
//		//	break;
//		//}
//
//		}
//
//	}
//
//}

//DWORD WINAPI WorkThread(LPVOID arg)
//{
//	for (auto it = g_MapRecvPlayerInfo.begin(); it != g_MapRecvPlayerInfo.end(); ++it) {
//
//	}
//}



//void UpdatePosition(DWORD _playerNumber)
//{
//	if (playerKeyInput[_playerNumber] & 0x0001) {
//		if (playerKeyInput[_playerNumber] & 0x0002) {
//			MoveForward(_playerNumber, (M_MoveForward_Speed / sqrtf(2.f)));
//			MoveLeft(_playerNumber, (M_MoveLeft_Speed / sqrtf(2.f)));
//		}
//		else if (playerKeyInput[_playerNumber] & 0x0008) {
//			MoveForward(_playerNumber, (M_MoveForward_Speed / sqrtf(2.f)));
//			MoveRight(_playerNumber, (M_MoveRight_Speed / sqrtf(2.f)));
//		}
//		else {
//			MoveForward(_playerNumber, M_MoveForward_Speed);
//		}
//	}
//	else if (playerKeyInput[_playerNumber] & 0x0004) {
//		if (playerKeyInput[_playerNumber] & 0x0002) {
//			MoveBackward(_playerNumber, (M_MoveBackward_Speed / sqrtf(2.f)));
//			MoveLeft(_playerNumber, (M_MoveLeft_Speed / sqrtf(2.f)));
//		}
//		else if (playerKeyInput[_playerNumber] & 0x0008) {
//			MoveBackward(_playerNumber, (M_MoveBackward_Speed / sqrtf(2.f)));
//			MoveRight(_playerNumber, (M_MoveRight_Speed / sqrtf(2.f)));
//		}
//		else {
//			MoveBackward(_playerNumber, M_MoveBackward_Speed);
//		}
//	}
//	else if (playerKeyInput[_playerNumber] & 0x0002) {
//		MoveLeft(_playerNumber, M_MoveLeft_Speed);
//	}
//	else if (playerKeyInput[_playerNumber] & 0x0008) {
//		MoveRight(_playerNumber, M_MoveRight_Speed);
//	}
//}

//void MoveForward(DWORD _playerNumber, float speed)
//{
//	XMFLOAT3 look;
//	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
//	memcpy(&look, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._21, sizeof(XMFLOAT3));
//	XMStoreFloat3(&l ook, XMVector3Normalize(XMLoadFloat3(&look)));
//	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));
//
//	XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&look) * speed);
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
//}
//
//void MoveBackward(DWORD _playerNumber, float speed)
//{
//	XMFLOAT3 look;
//	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
//	memcpy(&look, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._21, sizeof(XMFLOAT3));
//	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
//	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));
//
//	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&look) * speed);
//
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
//}
//
//void MoveLeft(DWORD _playerNumber, float speed)
//{
//	XMFLOAT3 left;
//	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
//	memcpy(&left, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._11, sizeof(XMFLOAT3));
//	XMStoreFloat3(&left, XMVector3Normalize(XMLoadFloat3(&left)));
//
//	XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&left) * speed);
//
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
//}
//
//void MoveRight(DWORD _playerNumber, float speed)
//{
//	XMFLOAT3 right;
//	XMFLOAT3 pos = { g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 ,g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 };
//	memcpy(&right, &g_MapRecvPlayerInfo[_playerNumber]->matWorld._11, sizeof(XMFLOAT3));
//	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
//
//	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&right) * speed);
//
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._41 = pos.x;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._42 = pos.y;
//	g_MapRecvPlayerInfo[_playerNumber]->matWorld._43 = pos.z;
//}
