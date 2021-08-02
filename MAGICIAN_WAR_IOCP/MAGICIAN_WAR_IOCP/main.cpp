#include "framework.h"
#include "Global.h"
#include "MeshMgr.h"
#include "StaticMeshMgr.h"
#include "Server.h"
#include "Room.h"
#include "PhysXMgr.h"

// extern
//vector<PxRigidStatic*> g_StaticObjects;
priority_queue<SOCKET> g_Accept_queue;
mutex g_Accept_mutex;

priority_queue<EVENT> g_Timer_queue;
mutex g_Time_mutex;

array<CLIENT_INFO*, MAX_CLIENTS> g_Clients; // �ִ� ������ŭ ����
mutex g_Client_mutex;
map<int,int> g_ConnectedClients_Number; // ����� Ŭ���̾�Ʈ�� User_num�� �������
map<int, Room*> g_Rooms;
mutex g_Room_mutex;

HANDLE g_IOCP; // IOCP �ڵ�
vector<PxRigidStatic*> StaticObjects;


void CreatePlayerSocket(int _size);

int main()
{
	// ������ ������� ���� ��� �۾����� �� �س�����
	StaticMeshMgr::GetInstance()->LoadMeshInfo("../../Data/Map1Data.txt");
	MeshMgr::GetInstnace()->BuildModels();
	//CPhysXMgr::GetInstance()->Initialize();

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

		//StaticObjects.push_back(CPhysXMgr::GetInstance()->CreateTriangleStaticMesh(*(*iter).first, world));
	}
	//cout << "StaticObjects Count is " << StaticObjects.size() << endl;
	cout << "All Load Complete!" << endl;

	CreatePlayerSocket(MAX_CLIENTS);

	Server::GetInstance()->Initialize();
	
	Server::GetInstance()->MainThread_Run(); // Only Accept

	Server::GetInstance()->DestoryInstance();
}

void CreatePlayerSocket(int _size)
{
	cout << " Ŭ���̾�Ʈ ���� ����(����) �̸� ���� \n";
	for (int user_num = 0; user_num < _size; ++user_num) {
		CLIENT_INFO* player = new CLIENT_INFO;
		ZeroMemory(player->Packet_buf, sizeof(char)*MAX_PACKET);
		ZeroMemory(player->Player_ID, sizeof(char) * MAX_ID_LEN);
		ZeroMemory(&player->Recv_over.Over, sizeof(player->Recv_over.Over));
		player->Recv_over.Wsabuf.len = MAX_BUFFER;
		player->Recv_over.Wsabuf.buf = reinterpret_cast<CHAR*>(player->Recv_over.Iocp_buf);
		player->User_num = user_num;
		player->Room_num = NO_ROOM;
		player->IsConnected = false; // ������
		player->Prev_data_size = 0;
		player->Cur_packet_size = 0;

		g_Clients[user_num] = player;

	}

	cout << " CreatePlayerSocket �Ϸ� \n";
}
