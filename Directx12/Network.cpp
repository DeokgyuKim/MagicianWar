#include "Network.h"

#include "MainApp.h"
#include "Scene.h"
#include "LobbyScene.h"
#include "RoomScene.h"
#include "TestScene.h"
#include "UI.h"
#include "Button.h"
#include "Player.h"
#include "PlayerFSM.h"
#include "Animation.h"
#include "KeyMgr.h"

#include "RadioButton.h"
#include "RoomRadioController.h"


Network* Network::m_pInstance = NULL;



bool Network::Init(const string& strServerIP)
{


	int retval;

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	// socket()
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sock == INVALID_SOCKET)
	{
		return false;
	}

	int option = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(strServerIP.c_str());
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(m_Sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
	{
		return false;
	}

	// nonBlocking
	unsigned long nonBlocking = 1;
	ioctlsocket(m_Sock, FIONBIO, &nonBlocking);


	CallEvent(EVENT_CTOS_CONNECT_OK, 0);

	/////////////////////////////////////////////


	// initData

	m_tMyInfo.xmfPosition = XMFLOAT3(0.f, 0.f, 0.f);
	m_tMyInfo.iHp = 100;
	m_tMyInfo.CharacterType = WIZARD_FIRE;
	m_tMyInfo.Room_Num = -1;

	packet_size = 0;
	savedPacket_size = 0;





	return false;
}

bool Network::Release()
{
	closesocket(m_Sock);
	WSACleanup();
	return false;
}

void Network::Update()
{
	// ���� ���⼭ ������
	recvUpdate();

	EventKeyInput();

	//switch (m_SceneChange)
	//{
	//case LOADING_SCENE:
	//	break;
	//case TITLE_SCENE:
	//	break;
	//case LOBBY_SCENE:
	//	break;
	//case ROOM_SCENE:
	//	break;
	//case STAGE_SCENE:
	//	break;
	//case RESULT_SCENE:
	//	break;
	//default:
	//	cout << "SCENE ERROR\n";
	//	break;
	//}

	//if (m_SceneChange == LOADING_SCENE)  // Logo
	//{

	//}
	//else if (m_SceneChange == LOBBY_SCENE) // Lobby
	//{
	//	//SendReadyState(); // lobby������ ready�� ����
	//}
	//else if (m_SceneChange == ROOM_SCENE)
	//{

	//}
	//else if (m_SceneChange == STAGE_SCENE) // mainScene
	//{
	//	//if (!mainSceneLateInit)
	//	//{
	//	//	//����� �÷��̾� ���� �޾ƿ���
	//	//	RecvOtherPlayerInfo();
	//	//	mainSceneLateInit = true;
	//	//}

	//	////Send
	//	////�� �÷��̾�����(��ġ, �ִϸ��̼ǹ���, Ÿ��, ��������, ����ġ)
	//	//SendMyPlayerInfo();
	//	////Ű�Է�����(w, a, s, d, ��ų12, ����, ���콺Ŭ��)
	//	//SendKeyInput();



	//}
	//else if (m_SceneChange == ENDING_Scene)
	//{
	//	//cout << "�������ΰ�" << endl;
	//}

}

int Network::recvn(SOCKET s, char* buf, int len, int flags)
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

void Network::Lobby_Init()
{
	for (int i = 0; i < 8; ++i) { // �濡 ���� Room�� �־��� �� ���� ������ Clear
		m_RoomPlayerSlots[i].characterType = ELEMENT_FIRE;
		m_RoomPlayerSlots[i].ishost = false;
		m_RoomPlayerSlots[i].used = false;
		m_RoomPlayerSlots[i].readyState = false;
	}
	m_tMyInfo.isreadyState = false;
	m_tMyInfo.isRoom_Host = false;
	m_tMyInfo.Room_Num = -1;
	m_tMyInfo.TeamType = TEAM_NONE;
	
}

void Network::Room_Init()
{
	// �濡 ���� ���� �������� ���� ���� ���� ���� ���� �䱸
	SendRoomInfo_Request();
}

void Network::Ingame_Init()
{
	SendIngameInfo_Request();
}

STOC_PlayerInfo Network::GetRecvPlayerInfo(DWORD playerNum)
{
	auto iter = m_mapRecvPlayerInfos.find(playerNum);
	if (iter == m_mapRecvPlayerInfos.end())
		return { 0 };
	return (*iter).second;
}

void Network::SetMyPlayerInfo(Player* pPlayer)
{

	XMFLOAT3 pos;
	int Root_Ani = 0;
	int Upper_Ani = 0;

	memcpy(&pos, &m_mapRecvPlayerInfos[m_tMyInfo.Client_Num].matWorld._41, sizeof(XMFLOAT3));
	//cout << pos.x << ", " << pos.y << ", " << pos.z << endl;
	Root_Ani = SCint(m_mapRecvPlayerInfos[m_tMyInfo.Client_Num].Root_eAnimType);
	Upper_Ani = SCint(m_mapRecvPlayerInfos[m_tMyInfo.Client_Num].Upper_eAnimType);
	//cout << "1�� Ŭ���� Root - " << Root_Ani << endl;
	//cout << "1�� Ŭ���� Upper - " << Root_Ani << endl;

	pPlayer->SetPosition(pos);
	dynamic_cast<AnimationCom*>(pPlayer->GetRootAniController())->ChangeAnimation(Root_Ani);
	dynamic_cast<AnimationCom*>(pPlayer->GetUpperAniController())->ChangeAnimation(Upper_Ani);
}

void Network::SetOtherPlayerInfo(list<Object*>* plstPlayer)
{

	//pPlayer->SetWorld(m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum].matWorld);
	for (auto iter = plstPlayer->begin(); iter != plstPlayer->end(); ++iter)
	{
		Player* pPlayer = dynamic_cast<Player*>(*iter);
		pPlayer->SetWorld(m_mapRecvPlayerInfos[pPlayer->GetNetworkInfo().Client_Num].matWorld);
	}


}

string Network::LoadServerIPtxt(string filePath)
{
	ifstream readFile;
	readFile.open(filePath, ios_base::in);

	if (!readFile.is_open())
		return "";

	string ServerIp;
	getline(readFile, ServerIp);

	readFile.close();
	return ServerIp;
}



void Network::CallEvent(int EventType, int args, ...)
{
	switch (EventType)
	{
	case EVENT_SCENE_CHANGE:
	{
		int scene;
		bool Retry = false;
		va_list vl;
		va_start(vl, args);
		scene = va_arg(vl, int);
		Retry = va_arg(vl, bool);
		va_end(vl);
		if (scene == TITLE_SCENE) {

		}
		else if (scene == LOBBY_SCENE) {
			LobbyScene* lobbyScene = new LobbyScene(Retry);
			MainApp::GetInstance()->ChangeScene(lobbyScene);
		}
		else if (scene == ROOM_SCENE) {
			RoomScene* Scene = new RoomScene();
			MainApp::GetInstance()->ChangeScene(Scene);
		}
		else if (scene == STAGE_SCENE) {
			TestScene* Scene = new TestScene();
			MainApp::GetInstance()->ChangeScene(Scene);
		}
		else if (scene == RESULT_SCENE) {

		}
		else if (scene == ENDING_SCENE) {

		}
	}
	break;
	// LOADING 
	case EVENT_LOADING_LOADINGEND:
	{
		SendLoadingEnd();
	}
	break;
	case EVENT_CTOS_CONNECT_OK:
	{
		SendConnectOK();
	}
	break;

	// TITLE
	case EVENT_TITLE_LOGIN:
		break;

		// LOBBY
	case EVENT_LOBBY_ROOMLIST_SHOW:
		break;
	case EVENT_LOBBY_ROOM_MAKE_REQUEST:
		SendRoomMake_Request();
		break;
	case EVENT_LOBBY_ROOM_JOIN_REQUEST:
		SendRoomJoin_Request();
		break;

		// ROOM
	case EVENT_ROOM_PLAYER_ENTER:
		break;
	case EVENT_ROOM_PLAYER_READY_ON:
		SendReadyState(true);
		break;
	case EVENT_ROOM_PLAYER_READY_OFF:
		SendReadyState(false);
		break;
	case EVENT_ROOM_PLAYER_SELECT_CHARACTER:
	{
		va_list ap;
		va_start(ap, args);
		int value = va_arg(ap, int);
		va_end(ap);
		SendCharacterChange_Request(value);
		break;
	}
	case EVENT_ROOM_PLAYER_EXIT:
	{
		SendExit_Request();
	}
		break;
	case EVENT_ROOM_PLAYER_SELECT_TEAM:
	{
		cout << "�� ����\n";
		va_list ap;
		va_start(ap, args);
		int value = va_arg(ap, int);
		va_end(ap);
		SendTeamChange_Request(value);
		break;
	}
		// STAGE
	case EVENT_STAGE_GAME_START:
		break;
	case EVENT_STAGE_INPUT_MOVE:
		break;
	case EVENT_STAGE_INPUT_BASIC_ATTACK:
		break;
	case EVENT_STAGE_PLAYER_INFO:
		break;
	case EVENT_STAGE_PLAYER_ANIMATE:
		break;

	default:
		break;
	}
}

bool Network::IsMoveToMainGame()
{
	bool isNext = false;
	int retval = recvn(m_Sock, (char*)&isNext, sizeof(bool), 0);

	cout << "IsNext? " << isNext << endl;

	return isNext;
}



void Network::recvUpdate() // ��� recv�� �̰� �Ұž� ����
{
	int retval = recv(m_Sock, recvBuffer, MAX_BUFFER, 0);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(m_Sock);
			WSACleanup();
		}

	}
	else if (retval == 0) return;
	else recvProcessing(retval);

}

void Network::recvProcessing(int _bytes)
{
	packet_start_ptr = recvBuffer;
	int recvBytes = _bytes;
	while (0 != recvBytes)
	{
		if (packet_size == 0)
			packet_size = *(reinterpret_cast<short*>(&packet_start_ptr[0]));

		if (recvBytes + savedPacket_size >= packet_size)
		{
			memcpy((packetBuffer + savedPacket_size), packet_start_ptr, (packet_size - savedPacket_size));

			packetProcessing(packetBuffer);

			packet_start_ptr += (packet_size - savedPacket_size);
			recvBytes -= (packet_size - savedPacket_size);
			packet_size = 0;
			savedPacket_size = 0;
		}
		else
		{
			memcpy((packetBuffer + savedPacket_size), packet_start_ptr, recvBytes);
			savedPacket_size += recvBytes;
			recvBytes = 0;
		}
	}

}

void Network::packetProcessing(char* _packetBuffer)
{
	unsigned char packetType = _packetBuffer[2];
	switch (packetType)
	{
	case stoc_Accept_OK: // Accept �Ϸ�
	{
		cout << "Server Accept OK" << endl;
		STOC_Accept_OK* data = reinterpret_cast<STOC_Accept_OK*>(_packetBuffer);
		m_tMyInfo.Client_Num = data->id;
		m_tMyInfo.TeamType = TEAM_NONE;
		m_tMyInfo.isRoom_Host = false;
		break;
	}
	//// Lobby
	case stoc_Room_Make_OK:
	{
		STOC_ROOM_MAKE_OK* data = reinterpret_cast<STOC_ROOM_MAKE_OK*>(_packetBuffer);
		cout << data->room_num << " ���� Room Make OK \n";
		if (m_tMyInfo.Client_Num == data->Host_num) { // �����̸� �Ѿ����
			m_tMyInfo.Room_Num = data->room_num;
			m_tMyInfo.TeamType = ELEMENT_FIRE;
			m_tMyInfo.isRoom_Host = true; // �� ��������� �����̰�
		}
		else { // ������ �ƴϰ� �κ� �ִ� �÷��̾�� �渮��Ʈ �����ؾ���
			m_mapRooms[data->room_num].Host_num = data->Host_num;
			m_mapRooms[data->room_num].room_num = data->room_num;
			m_mapRooms[data->room_num].size = data->size;
			m_mapRooms[data->room_num].type = data->type;
		}
		break;
	}
	case stoc_Room_Make_Deny:
	{
		cout << " Room Make Deny \n";
		break;
	}
	case stoc_Room_Join_OK:
	{
		cout << " Room Join OK \n";
		STOC_ROOM_JOIN* data = reinterpret_cast<STOC_ROOM_JOIN*>(_packetBuffer);
		m_tMyInfo.Room_Num = data->room_num;
		
		break;
	}
	case stoc_Room_Break_OK:
	{
		STOC_ROOM_BREAK* data = reinterpret_cast<STOC_ROOM_BREAK*>(_packetBuffer);
		int roomNum = data->room_num;

		cout << roomNum << " - Room Break" << endl;
		m_mapRooms.erase(data->room_num);
		break;
	}
	case stoc_Room_Join_Deny:
	{
		cout << " Room Join Deny \n";
		break;
	}
	// Room Scene
	case stoc_RoomPlayer_Enter: // Lobby ==> Room
	{
		cout << " �濡 ����\n";
		STOC_ROOM_ENTER* data = reinterpret_cast<STOC_ROOM_ENTER*>(_packetBuffer);
		m_tMyInfo.Room_Num = data->room_num; // ���� �� �� ��ȣ 
		CallEvent(EVENT_SCENE_CHANGE, 1, ROOM_SCENE);
		break;
	}
	case stoc_RoomPlayer_Change:
	{
		cout << "Room Slot Change \n";
		STOC_ROOM_CHANGE* data = reinterpret_cast<STOC_ROOM_CHANGE*>(_packetBuffer);
		int slot = data->slot_num;
		m_RoomPlayerSlots[slot].characterType = data->characterType;
		m_RoomPlayerSlots[slot].ishost = data->host;
		m_RoomPlayerSlots[slot].readyState = data->readyState;
		m_RoomPlayerSlots[slot].slot_num = data->slot_num;
		m_RoomPlayerSlots[slot].used = data->used;
		m_RoomPlayerSlots[slot].id = data->id;

		if (data->id == m_tMyInfo.Client_Num) { // �� ������ ���� ���� ��� �־����
			m_tMyInfo.CharacterType = data->characterType;
			m_tMyInfo.isRoom_Host = data->host;
			m_tMyInfo.isreadyState = data->readyState;
			m_tMyInfo.isRoom_Host = data->host;

			if (data->slot_num <4) {
				cout << "Blue Team \n";
			}
			else {
				cout << "Red Team \n";
			}
		}
		break;
	}
	case stoc_RoomPlayer_Leave: // Room == > Lobby
	{
		cout << " ���� ������\n";
		CallEvent(EVENT_SCENE_CHANGE, 2, LOBBY_SCENE, true);
		break;
	}
	// InGame
	case stoc_Game_Start:
	{
		cout << "���� ���� ��Ŷ ����\n";
		STOC_GAME_START* data = reinterpret_cast<STOC_GAME_START*>(_packetBuffer);

		Ingame_Init();
		
		break;
	}
	case stoc_InGame_StartInfo:
	{
		STOC_INGAME_STARTINFO* data = reinterpret_cast<STOC_INGAME_STARTINFO*>(_packetBuffer);
		cout << "pos - (" << data->xmfPosition.x << " , " << data->xmfPosition.y << " , " << data->xmfPosition.z << " )\n";
		cout << "data id - " << data->id << " , �� ���̵� - " <<m_tMyInfo.Client_Num << "\n";
		if (data->id == m_tMyInfo.Client_Num) {
			m_tMyInfo.iHp = data->iHp;
			m_tMyInfo.CharacterType = data->CharacterType;
			m_tMyInfo.TeamType = data->dwTeamNum;
			m_tMyInfo.xmfPosition = data->xmfPosition;
		}
		else {
			m_mapOtherPlayerInfos[data->id].Client_Num = data->id;
			m_mapOtherPlayerInfos[data->id].iHp = data->iHp;
			m_mapOtherPlayerInfos[data->id].CharacterType = data->CharacterType;
			m_mapOtherPlayerInfos[data->id].xmfPosition = data->xmfPosition;
			m_mapOtherPlayerInfos[data->id].TeamType = data->dwTeamNum;
		}
		break;
	}
	case stoc_SceneChange:
	{
		cout << "Scene Change\n";
		STOC_sceneChange* data = reinterpret_cast<STOC_sceneChange*>(_packetBuffer);
		CallEvent(EVENT_SCENE_CHANGE, 1, data->sceneType);
		break;
	}
	case stoc_otherPlayerNum: // �ٸ� �÷��̾� �ο��� �޾ƿ���
	{
		cout << "�ٸ� �÷��̾� �ο� �� �ޱ�" << endl;
		STOC_otherPlayerCount* data = reinterpret_cast<STOC_otherPlayerCount*>(_packetBuffer);
		m_iPlayerNum = data->playerCount + 1;
		break;
	}
	case stoc_OtherstartInfo:
	{
		cout << "�ٸ� �÷��̾� �ʱ���ǥ" << endl;
		STOC_OtherstartInfo* data = reinterpret_cast<STOC_OtherstartInfo*>(_packetBuffer);
		m_mapOtherPlayerInfos[data->dwPlayerNum].Client_Num = data->dwPlayerNum;
		m_mapOtherPlayerInfos[data->dwPlayerNum].TeamType = data->dwTeamNum;
		m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition = data->xmfPosition;
		m_mapOtherPlayerInfos[data->dwPlayerNum].iHp = data->iHp;
		m_mapOtherPlayerInfos[data->dwPlayerNum].CharacterType = data->CharacterType;
		//cout << m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition.x << ", "
		//	<< m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition.y << ", "
		//	<< m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition.z << endl;
		break;
	}
	case stoc_playerInfo:
	{
		//cout << "�츮 �÷��̾� ������Ʈ" << endl;
		STOC_PlayerInfo* data = reinterpret_cast<STOC_PlayerInfo*>(_packetBuffer);
		//cout << "�÷��̾� �ѹ� - " << data->playerInfo.dwPlayerNum << endl;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_eAnimBlendType = data->Root_eAnimBlendType;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fAnimTime = data->Root_fAnimTime;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fWeight = data->Root_fWeight;

		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].Root_eAnimType = data->Root_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].Upper_eAnimType = data->Upper_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].ePlayerState = data->ePlayerState;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].matWorld = data->matWorld;

		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.Client_Num = data->playerInfo.Client_Num;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.TeamType = data->playerInfo.TeamType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.xmfPosition = data->playerInfo.xmfPosition;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.iHp = data->playerInfo.iHp;
		//cout << "HP: " << data->playerInfo.iHp << endl;
		//cout << data->playerInfo.iHp << endl;


		//XMFLOAT3 pos;
		//memcpy(&pos, &m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].matWorld._41, sizeof(XMFLOAT3));

		//cout << "( " << pos.x << ", " << pos.y << ", " << pos.z << endl;
		break;
	}
	case stoc_bullet:
	{
		STOC_Bullet* data = reinterpret_cast<STOC_Bullet*>(_packetBuffer);
		m_vBullets.resize(data->Bullet_Count);
		for (int i = 0; i < data->Bullet_Count; ++i) {
			m_vBullets[i].id = data->bullets[i].id;
			m_vBullets[i].InstanceName = data->bullets[i].InstanceName;
			m_vBullets[i].lifeTime = data->bullets[i].lifeTime;
			m_vBullets[i].matWorld = data->bullets[i].matWorld;
		}
		//m_pBullets = data;
		//cout << "�Ѿ� ���� - " << (int)data->Bullet_Count << endl;
		//if (data->bullets != nullptr)
		//	cout << "�Ѿ� ù��° ģ�� ��ǥ - (" << data->bullets[0].matWorld._41 << ", " << data->bullets[0].matWorld._42 << ", " << data->bullets[0].matWorld._43 << " )" << endl;
		//cout << "ID : " << data->id << endl;
		//cout << "InstanceName : " << data->InstanceName << endl;
		//cout << "lifeTime : " << data->lifeTime << endl;
		//cout << "ID : " << data->id << endl;
		break;
	}

	case stoc_gameend:
	{
		STOC_GameEnd* data = reinterpret_cast<STOC_GameEnd*>(_packetBuffer);
		m_CLgameEnd.bEnd = data->bEnd;
		m_CLgameEnd.teamNum = data->teamNum;
		if (m_CLgameEnd.bEnd) cout << "������ �� ����" << endl;
		//else cout << "������ �ȳ�����" << endl;
		break;
	}
	default:
		cout << "No Type Packet" << endl;
		break;
	}

}

void Network::SendRoomMake_Request()
{
	CTOS_ROOM_MAKE_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_Room_Make_Request;
	if (!SendPacket(&packet)) {
		cout << "SendRoomMake_Request() Failed \n";
	}
}

void Network::SendRoomJoin_Request()
{
	CTOS_ROOM_JOIN_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_Room_Join_Request;
	packet.room_num = MainApp::GetInstance()->GetScene()->GetRadioButtonConteroller()->GetSelectRoomNumber();

	if (!SendPacket(&packet)) {
		cout << "SendRoomJoin_Request() Failed \n";
	}
}

void Network::SendRoomInfo_Request()
{
	CTOS_ROOMINFO_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_RoomInfo_Request;
	packet.room_num = m_tMyInfo.Room_Num; // ���� ���ϴ� ������ ���� ���� ��ȣ
	if (!SendPacket(&packet)) {
		cout << "SendRoomInfo_Request() Failed \n";
	}
}

void Network::SendReadyState(bool ReadyState)
{
	CTOS_Ready packet;
	packet.size = sizeof(CTOS_Ready);
	if (ReadyState) // ReadyOn
	{
		packet.type = ctos_Ready_OK;

	}
	else // Off
	{
		packet.type = ctos_Ready_NONE;
	}

	if (m_tMyInfo.isRoom_Host) { // ������ GameStart����
		packet.type = ctos_Game_Start;
	}

	if (!SendPacket(&packet)) {
		cout << "SendReadyState() Failed \n";
	}
}

void Network::SendExit_Request()
{
	CTOS_ROOM_EXIT packet;
	packet.size = sizeof(CTOS_ROOM_EXIT);
	packet.type = ctos_Room_Exit;
	if (!SendPacket(&packet)) {
		cout << "SendExit_Request() Failed \n";
	}
}

void Network::SendTeamChange_Request(int type)
{
	CTOS_TEAMSELECT_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_Team_Change;
	packet.teamType = static_cast<unsigned char>(type);
	if (!SendPacket(&packet)) {
		cout << "SendTeamChange_Request() Failed \n";
	}
}

void Network::SendCharacterChange_Request(int type)
{
	CTOS_CHARACTER_CHANGE packet;
	packet.size = sizeof(packet);
	packet.type = ctos_Character_Change;
	packet.characterType = static_cast<unsigned char>(type);
	if (!SendPacket(&packet)) {
		cout << "SendCharacterChange_Request() Failed \n";
	}
}

void Network::SendIngameInfo_Request()
{
	CTOS_INGAMEINFO_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_IngameInfo_Request;
	packet.room_num = m_tMyInfo.Room_Num;
	if (!SendPacket(&packet)) {
		cout << "SendIngameInfo_Request() Failed \n";
	}
}

void Network::SendMyPlayerInfo()
{
	//Object* pObj = MainApp::GetInstance()->GetScene()->GetPlayer();
	//
	//CTOS_PlayerInfo packet;
	//
	//if (pObj != NULL)
	//{
	//	Player* pPlayer = dynamic_cast<Player*>(MainApp::GetInstance()->GetScene()->GetPlayer());
	//	packet.matWorld = pPlayer->GetWorld();
	//	//tInfo_packet.ePlayerState = pPlayer->GetRootFSM()->GetState();
	//	packet.bAttackEnd = dynamic_cast<AnimationCom*>(pPlayer->GetUpperAniController())->GetAttackEnd();
	//	if (pPlayer->GetInstName() == CHARACTER_WIZARD_FIRE)
	//		packet.InstanceName = WIZARD_FIRE; // 01�� ĳ���� �޽��� ����� ģ����
	//	else if (pPlayer->GetInstName() == CHARACTER_WIZARD_COLD)
	//		packet.InstanceName = WIZARD_COLD;
	//	else if (pPlayer->GetInstName() == CHARACTER_WIZARD_DARKNESS)
	//		packet.InstanceName = WIZARD_DARKNESS;
	//}
	//else
	//{
	//	return;
	//	//tInfo_packet.matWorld = MathHelper::Identity4x4();
	//}
	//
	//packet.id = m_tMyInfo.Client_Num;
	//
	//int retval;
	//retval = send(m_Sock, (char*)&packet, packet.size, 0);
}

void Network::SendKeyInput()
{
	//
}

void Network::EventKeyInput()
{
	DWORD dwKeyInput = 0;
	if (KeyMgr::GetInstance()->KeyDown(0x31)) // ELEMENT_FIRE
	{ // 1
		dwKeyInput |= ctos_KEY_1;
	}
	if (KeyMgr::GetInstance()->KeyDown(0x32)) // ELEMENT_COLD
	{ // 2
		dwKeyInput |= ctos_KEY_2;
	}
	if (KeyMgr::GetInstance()->KeyDown(0x33)) // ELEMENT_DARKNESS
	{ // 3
		dwKeyInput |= ctos_KEY_3;
	}
	if (KeyMgr::GetInstance()->KeyPressing('W'))
	{
		dwKeyInput |= ctos_KEY_W;
	}
	if (KeyMgr::GetInstance()->KeyPressing('A'))
	{
		dwKeyInput |= ctos_KEY_A;
	}
	if (KeyMgr::GetInstance()->KeyPressing('S'))
	{
		dwKeyInput |= ctos_KEY_S;
	}
	if (KeyMgr::GetInstance()->KeyPressing('D'))
	{
		dwKeyInput |= ctos_KEY_D;
	}
	if (KeyMgr::GetInstance()->KeyPressing(VK_SPACE))
	{
		dwKeyInput |= ctos_KEY_SPACE;
	}
	if (KeyMgr::GetInstance()->KeyPressing(VK_LBUTTON))
	{
		dwKeyInput |= ctos_KEY_LBUTTON;
	}
	if (KeyMgr::GetInstance()->KeyPressing('E')) // FireRing
	{
		dwKeyInput |= ctos_KEY_E;
	}
}

void Network::SendLoadingEnd()
{
	CTOS_LoadingEnd packet;
	packet.size = sizeof(CTOS_LoadingEnd);
	packet.type = ctos_LoadingEnd;
	packet.id = m_tMyInfo.Client_Num;

	if (!SendPacket(&packet)) {
		cout << "SendLoadingEnd() Failed \n";
	}

}

void Network::SendConnectOK()
{
	CTOS_Connect_OK packet;
	packet.size = sizeof(CTOS_Connect_OK);
	packet.type = ctos_Connect_OK;
	if (!SendPacket(&packet)) {
		cout << "SendConnectOK() Failed \n";
	}
}

bool Network::SendPacket(void* buffer)
{
	char* packet = reinterpret_cast<char*>(buffer);
	int packetSize = (short)packet[0];
	int ret = send(m_Sock, packet, packetSize, 0);
	if (ret == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		error_display("SendPacket() - ", err_no);
		return false;
	}
	return true;
}

void Network::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"���� - " << err_no << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}

