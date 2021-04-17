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

		//다음 씬으로 넘어갈건지 확인
		if (Network::GetInstance()->IsMoveToMainGame())
		{
			//상대의 플레이어 정보 받아오기
			Network::GetInstance()->RecvOtherPlayerInfo();

			while (true)	///MainPlay
			{
				//Send
				//내 플레이어정보(위치, 애니메이션뭔지, 타임, 블레딩뭔지, 가중치)
				Network::GetInstance()->SendMyPlayerInfo();
				//키입력정보(w, a, s, d, 스킬12, 점프, 마우스클릭)
				Network::GetInstance()->SendKeyInput();

				//Recv
				//플레이어 정보들
				//Network::GetInstance()->RecvPlayerInfo();
				//스킬, 총알같은거 정보
				//게임상태 가져오기
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

