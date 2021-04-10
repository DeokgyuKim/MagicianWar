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



DWORD gClientNum = 0;
unordered_map<int, SERVERPLAYER*>		g_Clients;


int recvn(SOCKET s, char* buf, int len, int flags);

int main()
{
	int retval;

	// 윈속 초기화
	WSADATA	wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "윈속 초기화 안됨" << endl;
		return 1;
	}

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		cout << "socket 에러" << endl;
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
		cout << "bind 에러" << endl;
	}

	// listen()
	retval = listen(listen_sock, SOMAXCONN);			// 2명까지만 접속가능
	if (retval == SOCKET_ERROR)
	{
		cout << "listen 에러" << endl;
	}
	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (true)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		int option = TRUE;
		setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

		if (client_sock == INVALID_SOCKET)
		{
			cout << "accept 에러" << endl;
			break;
		}
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		g_Clients[gClientNum] = new SERVERPLAYER;
		g_Clients[gClientNum]->socket = client_sock;
		g_Clients[gClientNum]->info.dwPlayerNum = gClientNum;
		g_Clients[gClientNum]->info.dwTeamNum = gClientNum;	///변경해야함///

		int retval = send(g_Clients[gClientNum]->socket, (char*)&g_Clients[gClientNum]->info.dwPlayerNum, sizeof(DWORD), 0);
		retval = send(g_Clients[gClientNum]->socket, (char*)&g_Clients[gClientNum]->info.dwTeamNum, sizeof(DWORD), 0);
		
		gClientNum++;
	}

	closesocket(client_sock);

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}




























int recvn(SOCKET s, char* buf, int len, int flags)
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