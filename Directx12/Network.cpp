#include "Network.h"

Network* Network::m_pInstance = NULL;

unsigned int ReciveThreadForLobby(void* pArg)
{
	int retval;
	while (true)
	{
		//���� ������ �Ѿ���� Ȯ��
		if (Network::GetInstance()->IsMoveToMainGame())
		{
			//����� �÷��̾� ���� �޾ƿ���
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

