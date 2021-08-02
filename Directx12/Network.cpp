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


Network* Network::m_pInstance = NULL;



bool Network::Init(const string& strServerIP)
{
	m_bLobbyEnd = false;

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

	m_LateInit = false;
	{ // packet 초기화
		packetInit();
	}

	packet_size = 0;
	savedPacket_size = 0;
	m_SceneChange = LOADING_SCENE; // 처음엔 로딩씬
	mainSceneLateInit = false;


	return false;
}

void Network::packetInit()
{
	{ // packet 초기화
		{ // packet_keyInput
			KEY_packet.size = sizeof(KEY_packet);
			KEY_packet.type = packet_keyInput;
			KEY_packet.id = m_tMyInfo.Client_Num;
		}
		{ // ctos_playerInfo
			tInfo_packet.size = sizeof(tInfo_packet);
			tInfo_packet.type = ctos_playerInfo;
			tInfo_packet.ePlayerState = static_cast<int>(PLAYER_STATE::IDLE);
			tInfo_packet.bAttackEnd = false;

		}
		{// packet_ready
			Ready_packet.size = sizeof(Ready_packet);
			//Ready_packet.type = packet_ready;
			//Ready_packet.id = m_tMyInfo.Client_Num;
			//Ready_packet.CharacterType = WIZARD_FIRE;
			//Ready_packet.ready = 0;
		}

	}
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
	if (!m_LateInit) {
		packetInit();
		m_LateInit = true;
	}
	switch (m_SceneChange)
	{
	case LOADING_SCENE:
		break;
	case TITLE_SCENE:
		break;
	case LOBBY_SCENE:
		break;
	case ROOM_SCENE:
		break;
	case STAGE_SCENE:
		break;
	case RESULT_SCENE:
		break;
	default:
		cout << "SCENE ERROR\n";
		break;
	}

	if (m_SceneChange == LOADING_SCENE)  // Logo
	{

	}
	else if (m_SceneChange == LOBBY_SCENE) // Lobby
	{
		//SendReadyState(); // lobby에서는 ready만 보냄
	}
	else if (m_SceneChange == ROOM_SCENE)
	{

	}
	else if (m_SceneChange == STAGE_SCENE) // mainScene
	{
		//if (!mainSceneLateInit)
		//{
		//	//상대의 플레이어 정보 받아오기
		//	RecvOtherPlayerInfo();
		//	mainSceneLateInit = true;
		//}

		////Send
		////내 플레이어정보(위치, 애니메이션뭔지, 타임, 블레딩뭔지, 가중치)
		//SendMyPlayerInfo();
		////키입력정보(w, a, s, d, 스킬12, 점프, 마우스클릭)
		//SendKeyInput();



	}
	else if (m_SceneChange == ENDING_Scene)
	{
		//cout << "엔딩씬인가" << endl;
	}

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
	//cout << "1번 클라의 Root - " << Root_Ani << endl;
	//cout << "1번 클라의 Upper - " << Root_Ani << endl;

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

void Network::ClearNetworkForNext()
{
	//Ready_packet.ready = 0;
	m_SceneChange = LOBBY_SCENE;
}

void Network::CallEvent(int EventType, int args, ...)
{
	switch (EventType)
	{
	case EVENT_SCENE_CHANGE:
	{
		int scene;
		va_list vl;
		va_start(vl, args);
		scene = va_arg(vl, int);
		va_end(vl);
		if (scene == TITLE_SCENE) {

		}
		else if (scene == LOBBY_SCENE) {
			LobbyScene* lobbyScene = new LobbyScene();
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
		break;
	case EVENT_ROOM_PLAYER_READY_OFF:
		break;
	case EVENT_ROOM_PLAYER_SELECT_CHARACTER:
		break;
	case EVENT_ROOM_PLAYER_EXIT:
		break;

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

void Network::RecvOtherPlayerInfo()
{
	m_bLobbyEnd = true;
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
	char packetType = _packetBuffer[2];
	switch (packetType)
	{
	case stoc_Accept_OK: // Accept 완료
	{
		cout << "Server Accept OK" << endl;
		STOC_Accept_OK* data = reinterpret_cast<STOC_Accept_OK*>(_packetBuffer);
		m_tMyInfo.Client_Num = data->id;
		m_tMyInfo.dwTeamNum = TEAM_NONE;
		break;
	}
	case stoc_sceneChange: // IsMoveToMainGame
	{
		cout << "씬 전환" << endl;
		STOC_sceneChange* data = reinterpret_cast<STOC_sceneChange*>(_packetBuffer);
		m_SceneChange = data->sceneNum;
		if (m_SceneChange == LOBBY_Scene)
		{
			ClearNetworkForNext();
		}
		break;
	}
	case stoc_otherPlayerNum: // 다른 플레이어 인원수 받아오기
	{
		cout << "다른 플레이어 인원 수 받기" << endl;
		STOC_otherPlayerCount* data = reinterpret_cast<STOC_otherPlayerCount*>(_packetBuffer);
		m_iPlayerNum = data->playerCount + 1;
		break;
	}
	case stoc_OtherstartInfo:
	{
		cout << "다른 플레이어 초기좌표" << endl;
		STOC_OtherstartInfo* data = reinterpret_cast<STOC_OtherstartInfo*>(_packetBuffer);
		m_mapOtherPlayerInfos[data->dwPlayerNum].Client_Num = data->dwPlayerNum;
		m_mapOtherPlayerInfos[data->dwPlayerNum].dwTeamNum = data->dwTeamNum;
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
		//cout << "우리 플레이어 업데이트" << endl;
		STOC_PlayerInfo* data = reinterpret_cast<STOC_PlayerInfo*>(_packetBuffer);
		//cout << "플레이어 넘버 - " << data->playerInfo.dwPlayerNum << endl;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_eAnimBlendType = data->Root_eAnimBlendType;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fAnimTime = data->Root_fAnimTime;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fWeight = data->Root_fWeight;

		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].Root_eAnimType = data->Root_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].Upper_eAnimType = data->Upper_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].ePlayerState = data->ePlayerState;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].matWorld = data->matWorld;

		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.Client_Num = data->playerInfo.Client_Num;
		m_mapRecvPlayerInfos[data->playerInfo.Client_Num].playerInfo.dwTeamNum = data->playerInfo.dwTeamNum;
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
		//cout << "총알 개수 - " << (int)data->Bullet_Count << endl;
		//if (data->bullets != nullptr)
		//	cout << "총알 첫번째 친구 좌표 - (" << data->bullets[0].matWorld._41 << ", " << data->bullets[0].matWorld._42 << ", " << data->bullets[0].matWorld._43 << " )" << endl;
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
		if (m_CLgameEnd.bEnd) cout << "게임이 왜 끝나" << endl;
		//else cout << "게임이 안끝났어" << endl;
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
	//packet.type = .
}

void Network::SendRoomJoin_Request()
{
}

void Network::SendReadyState(int ReadyState)
{
	int retval;

	Object* pObj = MainApp::GetInstance()->GetScene()->GetUIForTag(0);

	if (pObj != NULL)
	{
		switch (dynamic_cast<Button*>(pObj)->GetButtonState())
		{
		case BUTTON_STATE::MOUSEON:
		case BUTTON_STATE::NONE:
			//Ready_packet.ready = 0;
			break;
		case BUTTON_STATE::ON:
			//Ready_packet.ready = 1;
			break;
		}
	}

	Object* pButton[3] = { MainApp::GetInstance()->GetScene()->GetUIForTag(WIZARD_FIRE),
		MainApp::GetInstance()->GetScene()->GetUIForTag(WIZARD_COLD),
		MainApp::GetInstance()->GetScene()->GetUIForTag(WIZARD_DARKNESS)
	};

	//if (pButton[0] != NULL && dynamic_cast<RadioButton*>(pButton[0])->GetButtonState() == BUTTON_STATE::ON)
	//	Ready_packet.CharacterType = WIZARD_FIRE;
	//else if (pButton[1] != NULL && dynamic_cast<RadioButton*>(pButton[1])->GetButtonState() == BUTTON_STATE::ON)
	//	Ready_packet.CharacterType = WIZARD_COLD;
	//else if (pButton[2] != NULL && dynamic_cast<RadioButton*>(pButton[2])->GetButtonState() == BUTTON_STATE::ON)
	//	Ready_packet.CharacterType = WIZARD_DARKNESS;

	retval = send(m_Sock, (char*)&Ready_packet, Ready_packet.size, 0);
}

void Network::SendMyPlayerInfo()
{
	Object* pObj = MainApp::GetInstance()->GetScene()->GetPlayer();


	if (pObj != NULL)
	{
		Player* pPlayer = dynamic_cast<Player*>(MainApp::GetInstance()->GetScene()->GetPlayer());
		tInfo_packet.matWorld = pPlayer->GetWorld();
		//tInfo_packet.ePlayerState = pPlayer->GetRootFSM()->GetState();
		tInfo_packet.bAttackEnd = dynamic_cast<AnimationCom*>(pPlayer->GetUpperAniController())->GetAttackEnd();
		if (pPlayer->GetInstName() == CHARACTER_WIZARD_FIRE)
			tInfo_packet.InstanceName = WIZARD_FIRE; // 01번 캐릭터 메쉬를 사용한 친구임
		else if (pPlayer->GetInstName() == CHARACTER_WIZARD_COLD)
			tInfo_packet.InstanceName = WIZARD_COLD;
		else if (pPlayer->GetInstName() == CHARACTER_WIZARD_DARKNESS)
			tInfo_packet.InstanceName = WIZARD_DARKNESS;
	}
	else
	{
		return;
		//tInfo_packet.matWorld = MathHelper::Identity4x4();
	}

	tInfo_packet.id = m_tMyInfo.Client_Num;

	int retval;
	retval = send(m_Sock, (char*)&tInfo_packet, tInfo_packet.size, 0);
}

void Network::SendKeyInput()
{
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

	KEY_packet.key = dwKeyInput;

	int retval;
	retval = send(m_Sock, (char*)&KEY_packet, KEY_packet.size, 0);
	int a = 0;
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
	std::wcout << L"에러 - " << err_no << lpMsgBuf << std::endl;

	LocalFree(lpMsgBuf);
}

