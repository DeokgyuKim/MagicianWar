#include "Network.h"

Network* Network::m_pInstance = NULL;

unsigned int ReciveThreadForLobby(void* pArg)
{
	int retval;
	while (true)
	{
		//다음 씬으로 넘어갈건지 확인
		if (Network::GetInstance()->IsMoveToMainGame())
		{
			//상대의 플레이어 정보 받아오기
			Network::GetInstance()->RecvOtherPlayerInfo();
			break;
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

	retval = recvn(m_Sock, (char*)&m_dwClientNum, sizeof(DWORD), 0);
	retval = recvn(m_Sock, (char*)&m_dwTeamNum, sizeof(DWORD), 0);


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

bool Network::IsMoveToMainGame()
{
	bool isNext = false;
	int retval = recvn(m_Sock, (char*)&isNext, sizeof(bool), 0);

	return isNext;
}

void Network::RecvOtherPlayerInfo()
{
	int retval;
	int OtherPlayerNum;
	retval = recvn(m_Sock, (char*)&OtherPlayerNum, sizeof(DWORD), 0);

	for (int i = 0; i < OtherPlayerNum; ++i)
	{
		PlayerInfo other;
		retval = recvn(m_Sock, (char*)&other, sizeof(PlayerInfo), 0);
		m_mapOtherPlayerInfos[other.dwPlayerNum] = other;
	}
	m_bLobbyEnd = true;
}

