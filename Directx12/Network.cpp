#include "Network.h"

#include "MainApp.h"
#include "Scene.h"
#include "UI.h"
#include "Button.h"
#include "Player.h"
#include "PlayerFSM.h"
#include "Animation.h"
#include "KeyMgr.h"


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

	retval = recv(m_Sock, recvBuffer, MAX_BUFFER, 0);
	if (retval > 0) recvProcessing(retval);

		// nonBlocking
		unsigned long nonBlocking = 1;
	ioctlsocket(m_Sock, FIONBIO, &nonBlocking);

	cout << m_tMyInfo.dwPlayerNum << ", " << m_tMyInfo.dwTeamNum << endl;

	{ // packet �ʱ�ȭ
		packetInit();
	}

	packet_size = 0;
	savedPacket_size = 0;
	m_SceneChange = 1; // ÷���� Lobby�� �ϴ�
	mainSceneLateInit = false;


	return false;
}

void Network::packetInit()
{
	{ // packet �ʱ�ȭ
		{ // packet_keyInput
			KEY_packet.size = sizeof(KEY_packet);
			KEY_packet.type = packet_keyInput;
			KEY_packet.id = m_tMyInfo.dwPlayerNum;
		}
		{ // ctos_playerInfo
			tInfo_packet.size = sizeof(tInfo_packet);
			tInfo_packet.type = ctos_playerInfo;
			tInfo_packet.ePlayerState = PLAYER_STATE::IDLE;
			tInfo_packet.bAttackEnd = false;

		}
		{// packet_ready
			Ready_packet.size = sizeof(Ready_packet);
			Ready_packet.type = packet_ready;
			Ready_packet.id = m_tMyInfo.dwPlayerNum;
			Ready_packet.ready = 0;
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
	// ���� ���⼭ ������
	recvUpdate();

	if (m_SceneChange == 0)  // Logo
	{

	}
	else if (m_SceneChange == 1) // Lobby
	{
		SendReadyState(); // lobby������ ready�� ����
	}
	else if (m_SceneChange == 2) // mainScene
	{
		if (!mainSceneLateInit)
		{
			//����� �÷��̾� ���� �޾ƿ���
			RecvOtherPlayerInfo();
			mainSceneLateInit = true;
		}
		//Send
		//�� �÷��̾�����(��ġ, �ִϸ��̼ǹ���, Ÿ��, ��������, ����ġ)
		SendMyPlayerInfo();
		//Ű�Է�����(w, a, s, d, ��ų12, ����, ���콺Ŭ��)
		SendKeyInput();
		//RecvPlayerInfo();

		//Object* pObj = MainApp::GetInstance()->GetScene()->GetPlayer();
		//if (pObj != nullptr) {
		//	Player* pPlayer = dynamic_cast<Player*>(MainApp::GetInstance()->GetScene()->GetPlayer());
		//	dynamic_cast<AnimationCom*>(pPlayer->GetRootAniController())->ChangeAnimation(SCint(m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum].Root_eAnimType));
		//	dynamic_cast<AnimationCom*>(pPlayer->GetUpperAniController())->ChangeAnimation(SCint(m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum].Upper_eAnimType));
		//}
	}

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

	memcpy(&pos, &m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum].matWorld._41, sizeof(XMFLOAT3));
	cout << pos.x << ", " << pos.y << ", " << pos.z << endl;
	Root_Ani = SCint(m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum].Root_eAnimType);
	Upper_Ani = SCint(m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum].Upper_eAnimType);
	cout << "1�� Ŭ���� Root - " << Root_Ani << endl;
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
		pPlayer->SetWorld(m_mapRecvPlayerInfos[pPlayer->GetNetworkInfo().dwPlayerNum].matWorld);
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
	char packetType = _packetBuffer[2];
	switch (packetType)
	{
	case stoc_startInfo: // �ʱ� recv 
	{
		cout << "�ʱ� ��ǥ �ޱ�" << endl;
		STOC_startInfo* data = reinterpret_cast<STOC_startInfo*>(_packetBuffer);
		m_tMyInfo.dwPlayerNum = data->dwPlayerNum;
		m_tMyInfo.dwTeamNum = data->dwTeamNum;
		m_tMyInfo.xmfPosition = data->xmfPosition;
		break;
	}
	case stoc_sceneChange: // IsMoveToMainGame
	{
		cout << "���ξ�����" << endl;
		STOC_sceneChange* data = reinterpret_cast<STOC_sceneChange*>(_packetBuffer);
		m_SceneChange = data->sceneNum; 
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
		m_mapOtherPlayerInfos[data->dwPlayerNum].dwPlayerNum = data->dwPlayerNum;
		m_mapOtherPlayerInfos[data->dwPlayerNum].dwTeamNum = data->dwTeamNum;
		m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition = data->xmfPosition;
		m_mapRecvPlayerInfos[data->dwPlayerNum];
		cout << m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition.x << ", "
			<< m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition.y << ", "
			<< m_mapOtherPlayerInfos[data->dwPlayerNum].xmfPosition.z << endl;
		break;
	}
	case stoc_playerInfo:
	{
		//cout << "�츮 �÷��̾� ������Ʈ" << endl;
		STOC_PlayerInfo* data = reinterpret_cast<STOC_PlayerInfo*>(_packetBuffer);
		cout << "�÷��̾� �ѹ� - " << data->playerInfo.dwPlayerNum << endl;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_eAnimBlendType = data->Root_eAnimBlendType;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fAnimTime = data->Root_fAnimTime;
		//m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_fWeight = data->Root_fWeight;
		
		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Root_eAnimType = data->Root_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].Upper_eAnimType = data->Upper_eAnimType;
		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].ePlayerState = data->ePlayerState;
		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].matWorld = data->matWorld;

		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].playerInfo.dwPlayerNum = data->playerInfo.dwPlayerNum;
		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].playerInfo.dwTeamNum = data->playerInfo.dwTeamNum;
		m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].playerInfo.xmfPosition = data->playerInfo.xmfPosition;


		//XMFLOAT3 pos;
		//memcpy(&pos, &m_mapRecvPlayerInfos[data->playerInfo.dwPlayerNum].matWorld._41, sizeof(XMFLOAT3));

		//cout << "( " << pos.x << ", " << pos.y << ", " << pos.z << endl;
		break;
	}

	}
}

void Network::SendReadyState()
{
	int retval;

	Object* pObj = MainApp::GetInstance()->GetScene()->GetUIForTag(0);

	if (pObj != NULL)
	{
		switch (dynamic_cast<Button*>(pObj)->GetButtonState())
		{
		case BUTTON_STATE::MOUSEON:
		case BUTTON_STATE::NONE:
			Ready_packet.ready = 0;
			break;
		case BUTTON_STATE::ON:
			Ready_packet.ready = 1;
			break;
		}
	}
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
	}
	else
	{
		tInfo_packet.matWorld = MathHelper::Identity4x4();
	}

	tInfo_packet.id = m_tMyInfo.dwPlayerNum;

	int retval;
	retval = send(m_Sock, (char*)&tInfo_packet, tInfo_packet.size, 0);
}

void Network::SendKeyInput()
{
	DWORD dwKeyInput = 0;

	if (KeyMgr::GetInstance()->KeyPressing('W'))
	{
		dwKeyInput |= 0x0001;
	}
	if (KeyMgr::GetInstance()->KeyPressing('A'))
	{
		dwKeyInput |= 0x0002;
	}
	if (KeyMgr::GetInstance()->KeyPressing('S'))
	{
		dwKeyInput |= 0x0004;
	}
	if (KeyMgr::GetInstance()->KeyPressing('D'))
	{
		dwKeyInput |= 0x0008;
	}
	if (KeyMgr::GetInstance()->KeyPressing(VK_SPACE))
	{
		dwKeyInput |= 0x0010;
	}
	if (KeyMgr::GetInstance()->KeyPressing(VK_LBUTTON))
	{
		dwKeyInput |= 0x0020;
	}

	KEY_packet.key = dwKeyInput;

	int retval;
	retval = send(m_Sock, (char*)&KEY_packet, KEY_packet.size, 0);
}

