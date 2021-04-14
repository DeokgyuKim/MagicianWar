#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#include "d3dUtil.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

#include "PlayerThread.h"
#include "struct.h"

//unordered_map<DWORD, bool> fds;

//unordered_map<int, SERVERPLAYER*>		g_Clients;
//vector<int> dsaf;

using namespace std;



unordered_map<DWORD, bool> ReadyCheck;
extern unordered_map<int, SERVERPLAYER*>		g_Clients;
unsigned int curScene = 1; // SCENE_LOGO





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


DWORD WINAPI PlayerThread(LPVOID arg)
{
	SERVERPLAYER* player = (SERVERPLAYER*)arg;
	SOCKET client_sock = player->socket;

	ReadyCheck[player->info.dwPlayerNum] = false;

	while (true) // loop
	{
		switch (curScene)
		{
		case 1:
		{
			DWORD buf;
			// ������¸� ��� �޾������
			recvn(client_sock, (char*)&buf, sizeof(buf), 0);
			if (buf == 1) { cout << "Ready �Ϸ�" << endl; }
			//else cout << "���� ����" << endl;
			cout << buf << endl;

			if (buf == 1)
				ReadyCheck[player->info.dwPlayerNum] = true; // �ش� �÷��̾�� ����
			else
				ReadyCheck[player->info.dwPlayerNum] = false; // �ش� �÷��̾�� ���� ����

			bool Ready = true;
			int PlayerCount = 0;
			for (auto it = ReadyCheck.begin(); it != ReadyCheck.end(); ++it) { // ��ü ��ȸ
				if (it->second == false) { // ���� ������ false���
					Ready = false; // ��ŸƮ ���ҰŰ�
				}
				++PlayerCount;
			}
			cout << Ready << endl;

			send(client_sock, (char*)&Ready, sizeof(Ready), 0); // ���� üũ�� ���� �����ְ�

			if (Ready) { // ��ȸ�ؼ� �� ������
				--PlayerCount; // �� �ڽ��� ��
				send(client_sock, (char*)&PlayerCount, sizeof(PlayerCount), 0); // �ο��� ������
				for (auto it = g_Clients.begin(); it != g_Clients.end(); ++it) {

					if (it->second->info.dwPlayerNum != player->info.dwPlayerNum) { // �� �ڽ� ���ܶ��
						PlayerInfo _pinfo;
						_pinfo = it->second->info;
						send(client_sock, (char*)&_pinfo, sizeof(_pinfo), 0); // �� �ڽ� ������ �ֵ��� playerinfo �Ѱ���
					}
				}
				curScene = 2; // ���� ��
			}
			break;
		}
		case 2:

		}

	}

}
