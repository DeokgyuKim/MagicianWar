#include "Network.h"

#include "MainApp.h"
#include "Scene.h"
#include "UI.h"
#include "Button.h"
#include "Player.h"

#include "KeyMgr.h"


Network* Network::m_pInstance = NULL;

unsigned int ReciveThreadForLobby(void* pArg)
{
	int retval;
	while (true)
	{
		Network::GetInstance()->SendReadyState();

		//���� ������ �Ѿ���� Ȯ��
		if (Network::GetInstance()->IsMoveToMainGame())
		{
			//����� �÷��̾� ���� �޾ƿ���
			Network::GetInstance()->RecvOtherPlayerInfo();

			while (true)	///MainPlay
			{
				//Send
				//�� �÷��̾�����(��ġ, �ִϸ��̼ǹ���, Ÿ��, ��������, ����ġ)
				Network::GetInstance()->SendMyPlayerInfo();
				//Ű�Է�����(w, a, s, d, ��ų12, ����, ���콺Ŭ��)
				Network::GetInstance()->SendKeyInput();

				//Recv
				//�÷��̾� ������
				//Network::GetInstance()->RecvPlayerInfo();
				//��ų, �Ѿ˰����� ����
				//���ӻ��� ��������
			}
		}
	}
	_endthreadex(0);

	return 0;
}

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

	retval = recvn(m_Sock, (char*)&m_tMyInfo.dwPlayerNum, sizeof(DWORD), 0);
	retval = recvn(m_Sock, (char*)&m_tMyInfo.dwTeamNum, sizeof(DWORD), 0);

	cout << m_tMyInfo.dwPlayerNum << ", " << m_tMyInfo.dwTeamNum << endl;


	m_hRecvThreadForLobby = (HANDLE)_beginthreadex(NULL, 0, ReciveThreadForLobby, this, 0, NULL);

    return false;
}

bool Network::Release()
{
    return false;
}

void Network::Update()
{
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

void Network::SetMyPlayerInfo(Player* pPlayer)
{
	//pPlayer->S
}

bool Network::IsMoveToMainGame()
{
	bool isNext = false;
	int retval = recvn(m_Sock, (char*)&isNext, sizeof(bool), 0);

	cout << "IsNext? " <<  isNext << endl;

	return isNext;
}

void Network::RecvOtherPlayerInfo()
{
	int retval;
	int OtherPlayerNum;

	retval = recvn(m_Sock, (char*)&m_tMyInfo, sizeof(PlayerInfo), 0);
	m_mapRecvPlayerInfos[m_tMyInfo.dwPlayerNum];
	cout << "Pos: " << m_tMyInfo.xmfPosition.x << ", " << m_tMyInfo.xmfPosition.y << ", " << m_tMyInfo.xmfPosition.z << endl;

	retval = recvn(m_Sock, (char*)&OtherPlayerNum, sizeof(DWORD), 0);

	cout << "PlayerNum: " << OtherPlayerNum + 1 << endl;
	m_iPlayerNum = OtherPlayerNum + 1;

	for (int i = 0; i < OtherPlayerNum; ++i)
	{
		PlayerInfo other;
		retval = recvn(m_Sock, (char*)&other, sizeof(PlayerInfo), 0);
		m_mapOtherPlayerInfos[other.dwPlayerNum] = other;
		m_mapRecvPlayerInfos[other.dwPlayerNum];
	}
	m_bLobbyEnd = true;
}

void Network::RecvPlayerInfo()
{
	int retval;
	SendToClientPlayerInfo tInfo;

	for (int i = 0; i < m_iPlayerNum; ++i)
	{
		retval = recvn(m_Sock, (char*)&tInfo, sizeof(SendToClientPlayerInfo), 0);
		m_mapRecvPlayerInfos[tInfo.playerInfo.dwPlayerNum] = tInfo;
	}
}

void Network::SendReadyState()
{
	int retval;

	Object* pObj = MainApp::GetInstance()->GetScene()->GetUIForTag(0);
	DWORD Send = 0;
	if (pObj != NULL)
	{
		switch (dynamic_cast<Button*>(pObj)->GetButtonState())
		{
		case BUTTON_STATE::MOUSEON:
		case BUTTON_STATE::NONE:
			Send = 0;
			break;
		case BUTTON_STATE::ON:
			Send = 1;
			break;
		}
	}
	retval = send(m_Sock, (char*)&Send, sizeof(DWORD), 0);
}

void Network::SendMyPlayerInfo()
{
	Object* pObj = MainApp::GetInstance()->GetScene()->GetPlayer();
	SendToServerPlayerInfo tInfo;

	if (pObj != NULL)
	{
		Player* pPlayer = dynamic_cast<Player*>(MainApp::GetInstance()->GetScene()->GetPlayer());
		tInfo.matWorld = pPlayer->GetWorld();
	}
	else
	{
		tInfo.matWorld = MathHelper::Identity4x4();
	}


	tInfo.ePlayerState = PLAYER_STATE::IDLE;
	tInfo.eAnimType = ANIMATION_TYPE::IDLE;
	tInfo.fAnimTime = 0.f;
	tInfo.eAnimBlendType = ANIMATION_TYPE::IDLE;
	tInfo.fWeight = 0.f;

	int retval;
	retval = send(m_Sock, (char*)&tInfo, sizeof(SendToServerPlayerInfo), 0);
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
		dwKeyInput |= 0x0003;
	}
	if (KeyMgr::GetInstance()->KeyPressing('D'))
	{
		dwKeyInput |= 0x0004;
	}

	int retval;
	retval = send(m_Sock, (char*)&dwKeyInput, sizeof(DWORD), 0);
}

