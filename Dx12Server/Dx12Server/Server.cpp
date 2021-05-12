#pragma once

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <list>
#include <random>
#include <time.h>
#include <mutex>
#include <thread>



#include "PlayerThread.h"
#include "Player.h"
#include "Bullet.h"

#include "include.h"
#include "MeshMgr.h"
#include "StaticMeshMgr.h"

int gClientNum = 0;

unsigned int WorkThreadCount = 0; // 1���� ��������

Player gClients[2];
vector<PxRigidStatic*> StaticObjects;

int recvn(SOCKET s, char* buf, int len, int flags);

int main()
{
	StaticMeshMgr::GetInstance()->LoadMeshInfo("../../Data/Map1Data.txt");
	MeshMgr::GetInstnace()->BuildModels();

	//Initailize PhysX
	CPhysXMgr::GetInstance()->Initialize();
	//for (int i = 0; i < gClientNum; ++i)
	//{
	//	gClients[i].CreateCapsuleController();
	//}
	multimap<string*, TransformStruct> vtxs = StaticMeshMgr::GetInstance()->GetMapMeshInfo();
	for (auto iter = vtxs.begin(); iter != vtxs.end(); ++iter)
	{
		XMFLOAT3 scale = (*iter).second.xmfScale;
		XMFLOAT3 rotate = (*iter).second.xmfRotate;
		XMFLOAT3 trans = (*iter).second.xmfPosition;

		XMMATRIX	s, rx, ry, rz, t, world;
		s = XMMatrixScaling(scale.x, scale.y, scale.z);
		rx = XMMatrixRotationX(XMConvertToRadians(rotate.x - 90.f));
		ry = XMMatrixRotationY(XMConvertToRadians(rotate.y + 90.f));
		rz = XMMatrixRotationZ(XMConvertToRadians(rotate.z));
		t = XMMatrixTranslation(trans.x, trans.y, trans.z);

		world = s * rx * ry * rz * t;

		StaticObjects.push_back(CPhysXMgr::GetInstance()->CreateTriangleStaticMesh(*(*iter).first, world));
	}
	cout << "StaticObjects Count is " << StaticObjects.size() << endl;
	cout << "All Load Complete!" << endl;

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
	auto workThread = thread{ WorkThread }; // workThread ����
	thread playerThread[2]; // playerThread
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE ThreadHandler;



	while (gClientNum < 2) // �켱 1�� ���� ���
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		//int option = TRUE;
		//setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

		if (client_sock == INVALID_SOCKET)
		{
			cout << "accept ����" << endl;
			break;
		}

		{ // 0501
			gClients[gClientNum].Initialize(client_sock, gClientNum);
			playerThread[gClientNum] = thread(PlayerThread, gClients[gClientNum].getInfo());

			STOC_startInfo packet;
			packet.size = sizeof(packet);
			packet.type = stoc_startInfo; // �ù� �̰� �� �ȶ� 
			packet.dwTeamNum = gClients[gClientNum].getInfo().info.dwTeamNum;
			packet.dwPlayerNum = gClients[gClientNum].getInfo().info.dwPlayerNum;
			packet.xmfPosition = XMFLOAT3(20.f, 0.f, 20.f);
			packet.iHp = 100;
			// playernum ������ teamnum ������
			gClients[gClientNum].sendPacket((void*)(&packet), sizeof(packet));			
		}
	gClientNum++;
	}

	// �ٸ� ��������� ���� ���������� ���
	for (auto& pThread : playerThread)
		pThread.join();
	workThread.join();

	// ���� ����
	WSACleanup();

	return 0;
}




























