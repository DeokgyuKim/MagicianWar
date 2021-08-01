#pragma once

#include "framework.h"

constexpr int MAX_BUFFER = 4096;
constexpr int MAX_PACKET = 256;
constexpr int WORKTHREAD_COUNT = 6; // �� ������ ���� == �ھ� ���� * 1.5 ( Main

constexpr int MAX_PLAYER = 8; // �ִ� 4 : 4 ���� ����
constexpr int MAX_ID_LEN = 10;
constexpr int MAX_CLIENTS = 100; // ( �׽�Ʈ�� 100�� ���� ) �̰� �ִ�� �ϸ鼭 ������ ����ȭ �Ǵ°� IOCP�� ����
constexpr int MAX_ROOMS = 50; // 1 : 1 ����

constexpr int NO_PLAYER = -1;
constexpr int NO_ROOM = -1;
constexpr int NO_ROOM_PACKET = -1;

constexpr int ACTIVATE = 1;
constexpr int DISABLED = -1;


enum OP_TYPE { // Accept�� Main_Thread�� �Ұ���
	OP_RECV, OP_SEND, // WorkThread
	OP_ACCEPT, // MainThread

	OP_ROOM_UPDATE, OP_ROOM_TIME, // GameRoom
	OP_ROOM_SEND_PACKET
};

struct EVENT {
	int Object_ID;
	int Target_ID;
	std::chrono::system_clock::time_point wakeup_time;
	OP_TYPE opType;

	constexpr bool operator<(const EVENT& _rhs) const { // typedef�� operator �ȵǳ�?
		return wakeup_time > _rhs.wakeup_time;
	}
};

typedef struct MyOverlapped_EX { // 
	WSAOVERLAPPED Over;
	WSABUF Wsabuf;
	char Iocp_buf[MAX_BUFFER];
	OP_TYPE OpType;
}OVER_EX;

typedef struct Client_Info { // Ŭ���̾�Ʈ ���� ����ü
	mutex Client_mutex;
	SOCKET Socket{};
	OVER_EX Recv_over;
	
	bool IsConnected;
	int User_num; // ������ ���� ID
	int Room_num; // ������ ����

	char Packet_buf[MAX_PACKET]; // ��Ŷ�� �󸶳� ���� �� �ֳ�
	int Prev_data_size; // �������� ���� ��Ŷ�� ������
	int Cur_packet_size; // ���� ���� ��Ŷ�� ������?

	char Player_ID[MAX_ID_LEN + 1]{}; // ���� ���̵�
}CLIENT_INFO;

struct ROOM_EVENT {
	int playerID; // � �÷��̾��� 
	char type;		// ���� ��Ŷ�ΰ�

	// ��Ÿ ����
};

struct Send_Packet_RoomInfo {
	int playerID;
	char buffer[MAX_PACKET];
};