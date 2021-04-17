#pragma once

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#include <winsock2.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <list>
#include <random>
#include <time.h>

using namespace std;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")
#include "d3dUtil.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

#include "struct.h"
#include "define.h"
#include "PlayerThread.h"

DWORD gClientNum = 0;
unordered_map<int, SERVERPLAYER*>		g_Clients;
unsigned int WorkThreadCount = 0; // 1���� ��������


int recvn(SOCKET s, char* buf, int len, int flags);

int main()
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA	wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "���� �ʱ�ȭ �ȵ�" << endl;
		return 1;
	}

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		cout << "socket ����" << endl;
	}

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		cout << "bind ����" << endl;
	}

	// listen()
	retval = listen(listen_sock, SOMAXCONN);			// 2������� ���Ӱ���
	if (retval == SOCKET_ERROR)
	{
		cout << "listen ����" << endl;
	}
	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE ThreadHandler;

	while (true)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		int option = TRUE;
		setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

		if (client_sock == INVALID_SOCKET)
		{
			cout << "accept ����" << endl;
			break;
		}
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		g_Clients[gClientNum] = new SERVERPLAYER; // Ŭ�� �ϳ� ���ö�����
		g_Clients[gClientNum]->socket = client_sock;
		g_Clients[gClientNum]->info.dwPlayerNum = gClientNum; // �÷��̾��� �ѹ�
		g_Clients[gClientNum]->info.dwTeamNum = gClientNum;	///�����ؾ���///

		ThreadHandler = CreateThread(NULL, 0, PlayerThread, (LPVOID)g_Clients[gClientNum], 0, NULL);
		if (ThreadHandler == NULL) { closesocket(g_Clients[gClientNum]->socket); }
		else { CloseHandle(ThreadHandler); }

		//if (!WorkThreadCount) {
		//	CreateThread(NULL, 0, WorkThread, 0, 0, NULL);
		//	++WorkThreadCount;
		//}


		int retval = send(g_Clients[gClientNum]->socket, (char*)&g_Clients[gClientNum]->info.dwPlayerNum, sizeof(DWORD), 0);
		retval = send(g_Clients[gClientNum]->socket, (char*)&g_Clients[gClientNum]->info.dwTeamNum, sizeof(DWORD), 0);

		gClientNum++;
	}

	//closesocket(client_sock);

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();

	return 0;
}




























