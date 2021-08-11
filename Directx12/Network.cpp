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
#include "Camera.h"
#include "ShopController.h"

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
	m_CurRound = 0;
	m_isRoundStart = false;



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
	// 이제 여기서 돌릴거
	recvUpdate();

	if (m_Curscene == STAGE_SCENE)
		ServerKeyInput();


}

int Network::recvn(SOCKET s, char* buf, int len, int flags)
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

void Network::Lobby_Init()
{
	for (int i = 0; i < 8; ++i) { // 방에 들어가면 Room에 있었을 때 썻던 정보를 Clear
		m_RoomPlayerSlots[i].characterType = ELEMENT_FIRE;
		m_RoomPlayerSlots[i].ishost = false;
		m_RoomPlayerSlots[i].used = false;
		m_RoomPlayerSlots[i].readyState = false;
	}
	m_tMyInfo.isreadyState = false;
	m_tMyInfo.isRoom_Host = false;
	m_tMyInfo.Room_Num = -1;
	m_tMyInfo.TeamType = TEAM_NONE;

	m_Curscene = LOBBY_SCENE;
	SendRoomlist_Request();
}

void Network::Room_Init()
{
	// 방에 들어가면 먼저 서버에게 지금 내가 속한 방의 정보 요구
	m_Curscene = ROOM_SCENE;
	m_RoundEnd.WinnerTeam = TEAM_NONE;
	m_isRoundStart = false;
	SendRoomInfo_Request();
}

void Network::Ingame_Init()
{
	m_Curscene = STAGE_SCENE;
	m_prevKey = 0;
	m_tMyInfo.killPoint = 0; // ingame 돌입시 0
	m_vBullets.resize(BulletCB_Count);
	for (int i = 0; i < BulletCB_Count; ++i) {
		m_vBullets[i].Used = false;
	}
	SendIngameInfo_Request();
}

STOC_PlayerInfo Network::GetRecvPlayerInfo(DWORD playerNum)
{
	auto iter = m_mapRecvPlayerInfos.find(playerNum);
	if (iter == m_mapRecvPlayerInfos.end())
		return { 0 };
	return (*iter).second;
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
		break;
	}
	break;
	case EVENT_CTOS_CONNECT_OK:
	{
		SendConnectOK();
		break;
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
		cout << "팀 고르기\n";
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
	{
		va_list ap;
		va_start(ap, args);
		bool value = va_arg(ap, bool);
		va_end(ap);
		SendAttackEnd(value);
		break;
	}
	case EVENT_STAGE_CAMERA_UPDATE:
	{
		va_list ap;
		va_start(ap, args);
		double value = va_arg(ap, double);
		va_end(ap);
		SendCameraUpdate(value);
		break;
	}
	case EVENT_ROUND_SHOPPING_START_REQUEST:
	{
		SendShoppingStart_Request();
		break;
	}
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



void Network::recvUpdate() // 모든 recv는 이걸 할거야 이제
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
	case stoc_Accept_OK: // Accept 완료
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
		cout << data->room_num << " 번방 Room Make OK \n";
		if (m_tMyInfo.Client_Num == data->Host_num) { // 방장이면 넘어가야지
			m_tMyInfo.Room_Num = data->room_num;
			m_tMyInfo.TeamType = ELEMENT_FIRE;
			m_tMyInfo.isRoom_Host = true; // 방 만들었으니 방장이고
			//CallEvent(EVENT_SCENE_CHANGE, 1, ROOM_SCENE);
		}
		//else { // 방장이 아니고 로비에 있는 플레이어면 방리스트 갱신해야함
		MakeRoom_By_roomNum(data->room_num);
		//}
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
		//CallEvent(EVENT_SCENE_CHANGE, 1, ROOM_SCENE);
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
	case stoc_RoomList_Update:
	{

		STOC_ROOMLIST_UPDATE* data = reinterpret_cast<STOC_ROOMLIST_UPDATE*>(_packetBuffer);
		MakeRoom_By_roomNum(data->room_num);
	}
	case stoc_Room_Join_Deny:
	{
		cout << " Room Join Deny \n";
		break;
	}
	// Room Scene
	case stoc_RoomPlayer_Enter: // Lobby ==> Room
	{
		cout << " 방에 들어갔음\n";
		STOC_ROOM_ENTER* data = reinterpret_cast<STOC_ROOM_ENTER*>(_packetBuffer);
		m_tMyInfo.Room_Num = data->room_num; // 내가 들어간 방 번호 
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

		if (data->id == m_tMyInfo.Client_Num) { // 내 정보는 나도 따로 들고 있어야지
			m_tMyInfo.CharacterType = data->characterType;
			m_tMyInfo.isRoom_Host = data->host;
			m_tMyInfo.isreadyState = data->readyState;


			if (data->slot_num < 4) {
				cout << "Blue Team \n";
			}
			else {
				cout << "Red Team \n";
			}
		}

		Scene* pScene = MainApp::GetInstance()->GetScene();
		if (pScene->GetSceneType() == SCENE_TYPE::ROOM)
		{
			RoomScene* pRoom = dynamic_cast<RoomScene*>(pScene);
			for (int i = 0; i < 8; ++i)
			{
				if (i == 0)
				{
					if (m_RoomPlayerSlots[i].used)
						cout << "룸플레이어 슬롯 0번이 사용되고있어." << endl;
					else
						cout << "룸플레이어 슬롯 0번이 사용안되고있어." << endl;
				}
				Ui_Roomplayer playerinfo;
				playerinfo.used = m_RoomPlayerSlots[i].used;
				playerinfo.host = m_RoomPlayerSlots[i].ishost;
				playerinfo.readystate = m_RoomPlayerSlots[i].readyState;
				playerinfo.chartype = m_RoomPlayerSlots[i].characterType;
				pRoom->SetRoomPlayer(i, playerinfo);
			}
		}

		break;
	}
	case stoc_RoomPlayer_Leave: // Room == > Lobby
	{
		cout << " 방을 나갔음\n";
		CallEvent(EVENT_SCENE_CHANGE, 2, LOBBY_SCENE, true);
		break;
	}
	// InGame
	case stoc_Game_Start:
	{
		cout << "게임 시작 패킷 받음\n";
		STOC_GAME_START* data = reinterpret_cast<STOC_GAME_START*>(_packetBuffer);

		Ingame_Init();

		break;
	}
	case stoc_InGame_StartInfo:
	{
		STOC_INGAME_STARTINFO* data = reinterpret_cast<STOC_INGAME_STARTINFO*>(_packetBuffer);
		cout << "pos - (" << data->xmfPosition.x << " , " << data->xmfPosition.y << " , " << data->xmfPosition.z << " )\n";
		cout << "data id - " << data->id << " , 내 아이디 - " << m_tMyInfo.Client_Num << "\n";
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
	case stoc_otherPlayerNum: // 다른 플레이어 인원수 받아오기
	{
		cout << "다른 플레이어 인원 수 받기" << endl;
		STOC_otherPlayerCount* data = reinterpret_cast<STOC_otherPlayerCount*>(_packetBuffer);
		m_iPlayerNum = data->playerCount + 1;
		break;
	}
	case stoc_playerInfo:
	{
		//cout << "우리 플레이어 업데이트" << endl;
		STOC_PlayerInfo* data = reinterpret_cast<STOC_PlayerInfo*>(_packetBuffer);
		//cout << "플레이어 넘버 - " << data->playerInfo.dwPlayerNum << endl;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_eAnimBlendType = data->Root_eAnimBlendType;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fAnimTime = data->Root_fAnimTime;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fWeight = data->Root_fWeight;

		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].Root_eAnimType = data->Root_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].Upper_eAnimType = data->Upper_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].ePlayerState = data->ePlayerState;

		if (data->playerInfo.Client_Num == m_tMyInfo.Client_Num) {
			m_tMyInfo.PlayerState = data->ePlayerState;
		}
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.Client_Num = data->playerInfo.Client_Num;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.TeamType = data->playerInfo.TeamType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.xmfPosition = data->playerInfo.xmfPosition;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.iHp = data->playerInfo.iHp;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.CameraY = data->playerInfo.CameraY;
		//cout << "HP: " << data->playerInfo.iHp << endl;
		//cout << data->playerInfo.iHp << endl;


		//XMFLOAT3 pos;
		//memcpy(&pos, &m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].matWorld._41, sizeof(XMFLOAT3));

		//cout << "( " << pos.x << ", " << pos.y << ", " << pos.z << endl;
		break;
	}
	case stoc_bullet_update:
	{
		//cout << "총알 업데이트\n";
		STOC_Bullet_Update* data = reinterpret_cast<STOC_Bullet_Update*>(_packetBuffer);

		int index = data->index;
		m_vBullets[index].Used = true;
		m_vBullets[index].ElementType = data->ElementType;
		m_vBullets[index].matWorld = data->xmmWorld;

		break;
	}
	case stoc_bullet_delete:
	{
		cout << " 총알 소멸\n";
		STOC_Bullet_Delete* data = reinterpret_cast<STOC_Bullet_Delete*>(_packetBuffer);
		int index = data->index;
		m_vBullets[index].Used = false;
		break;
	}

	case stoc_roundend:
	{
		STOC_RoundEnd* data = reinterpret_cast<STOC_RoundEnd*>(_packetBuffer);
		m_RoundEnd.WinnerTeam = data->teamType;	
		break;
	}
	case stoc_left_shopping_time:
	{
		STOC_LEFT_SHOPPING_TIME* data = reinterpret_cast<STOC_LEFT_SHOPPING_TIME*>(_packetBuffer);
		m_iTimeLeft = (short)data->leftTime;
		break;
	}
	case stoc_roundstart:
	{
		STOC_ROUND_START* data = reinterpret_cast<STOC_ROUND_START*>(_packetBuffer);
		cout << data->Cur_Round << " 라운드가 시작됩니다.\n";
		m_CurRound = data->Cur_Round;
		m_isRoundStart = true;
		m_RoundEnd.WinnerTeam = TEAM_NONE;
		dynamic_cast<TestScene*>(MainApp::GetInstance()->GetScene())->SetGameEndForPanelCreate(false);
		Object* pobj = MainApp::GetInstance()->GetScene()->GetUIForTag(99);
		if (pobj != nullptr)
		{
			MainApp::GetInstance()->GetScene()->RemoveObject(pobj, OBJ_TYPE::OBJ_UI);
			delete pobj;
		}
		dynamic_cast<Camera*>(MainApp::GetInstance()->GetScene()->GetCamera())->SetMode(CAMERA_MODE::CAMERA_THIRD);
		ShopController::GetInstance()->SetRendering(false);
		break;
	}
	case stoc_roundreset:
	{
		if (Network::GetInstance()->GetMyInfo().isRoom_Host)
			Network::GetInstance()->CallEvent(EVENT_ROUND_SHOPPING_START_REQUEST, 0);
		break;
	}
	case stoc_add_kill_point:
	{
		cout << "킬 포인트 올라감\n";
		++m_tMyInfo.killPoint;
		dynamic_cast<Player*>(MainApp::GetInstance()->GetScene()->GetPlayer())->AddKillCount();
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

void Network::MakeRoom_By_roomNum(int room_num)
{
	//m_mapRooms[room_num].Host_num = data->Host_num;
	m_mapRooms[room_num].room_num = room_num;

}

void Network::SendRoomInfo_Request()
{
	CTOS_ROOMINFO_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_RoomInfo_Request;
	packet.room_num = m_tMyInfo.Room_Num; // 내가 원하는 정보를 가진 방의 번호
	if (!SendPacket(&packet)) {
		cout << "SendRoomInfo_Request() Failed \n";
	}
}

void Network::SendRoomlist_Request()
{
	CTOS_ROOMLIST_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_Room_List_Request;
	if (!SendPacket(&packet)) {
		cout << "SendRoomlist_Request() Failed \n";
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

	if (m_tMyInfo.isRoom_Host) { // 방장은 GameStart보냄
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

void Network::SendCameraUpdate(float cameraY)
{
	// 죽으면 카메라 회전 안받음
	if (m_tMyInfo.PlayerState == STATE_DEAD || 
		m_tMyInfo.PlayerState == STATE_DANCE) return;
	// 라운드 시작 안하면 카메라 회전 안받음
	if (!m_isRoundStart) return;

	CTOS_CAMERA packet;
	packet.size = sizeof(packet);
	packet.type = ctos_Camera_y;
	packet.CameraY = cameraY;
	//cout << "카메라 y - " << cameraY<< " ";
	if (!SendPacket(&packet)) {
		cout << "SendCameraUpdate() Failed \n";
	}
}

void Network::SendAttackEnd(bool _bAttack)
{
	CTOS_ATTACKEND packet;
	packet.size = sizeof(packet);
	packet.type = ctos_AttackEnd;
	packet.isAttacked = _bAttack;
	if (!SendPacket(&packet)) {
		cout << "SendAttackEnd() Failed \n";
	}

}

void Network::SendKeyInput(DWORD _keyInput)
{
	CTOS_KEYINPUT packet;
	packet.size = sizeof(packet);
	packet.type = ctos_keyInput;
	packet.key = _keyInput;
	if (!SendPacket(&packet)) {
		cout << "SendKeyInput() Failed \n";
	}
}

void Network::ServerKeyInput()
{
	// 죽으면 키입력 안받음
	if (m_tMyInfo.PlayerState == STATE_DEAD || 
		m_tMyInfo.PlayerState == STATE_DANCE) return;
	// 라운드 시작 아니면 키입력 안받음
	if (!m_isRoundStart) return;

	DWORD dwKeyInput = 0;

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

	if (m_prevKey != dwKeyInput) {
		m_prevKey = dwKeyInput;
		SendKeyInput(m_prevKey);
	}



}

void Network::SendShoppingStart_Request()
{
	m_isRoundStart = false;

	CTOS_SHOPPINGSTART_REQUEST packet;
	packet.size = sizeof(packet);
	packet.type = ctos_ShoppingStart_Request;
	if (!SendPacket(&packet)) {
		cout << "SendShoppingStart_Request() Failed \n";
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
	int packetSize = *(short*)(&packet[0]);
	int ret = send(m_Sock, packet, (int)packetSize, 0);
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
	std::wcout << L"에러 - " << err_no << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}

