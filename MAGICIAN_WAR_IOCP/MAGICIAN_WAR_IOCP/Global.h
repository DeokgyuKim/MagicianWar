#pragma once

#include "framework.h"

constexpr int MAX_BUFFER = 4096;
constexpr int MAX_PACKET = 256;
constexpr int WORKTHREAD_COUNT = 6; // 총 쓰레드 개수 == 코어 개수 * 1.5 ( Main

constexpr int MAX_PLAYER = 8; // 최대 4 : 4 까지 고려
constexpr int MAX_ID_LEN = 10;
constexpr int MAX_CLIENTS = 100; // ( 테스트로 100명 동접 ) 이걸 최대로 하면서 성능이 안정화 되는게 IOCP의 목적
constexpr int ROOM_INDEX_START = 100;
constexpr int MAX_ROOMS = 50; // 1 : 1 고려

constexpr int NO_PLAYER = -1;
constexpr int NO_ROOM = -1;
constexpr int NO_ROOM_PACKET = -1;

constexpr int ACTIVATE = 1;
constexpr int DISABLED = -1;

// Character Default MoveSpeed
#define P_MoveForward_Speed 0.003f
#define P_MoveLeft_Speed 0.003f
#define P_MoveRight_Speed 0.003f
#define P_MoveBackward_Speed 0.003f

enum OP_TYPE { // Accept는 Main_Thread가 할거임
	OP_RECV, OP_SEND, // WorkThread
	OP_ACCEPT, // MainThread

	OP_ROOM_UPDATE, OP_ROOM_TIME, // GameRoom
	OP_ROOM_SEND_PACKET,OP_ROOM_BREAK
};

struct EVENT {
	int Object_ID;
	int Target_ID;
	std::chrono::system_clock::time_point wakeup_time;
	OP_TYPE opType;

	constexpr bool operator<(const EVENT& _rhs) const { // typedef는 operator 안되네?
		return wakeup_time > _rhs.wakeup_time;
	}
};

typedef struct MyOverlapped_EX { // 
	WSAOVERLAPPED Over;
	WSABUF Wsabuf;
	char Iocp_buf[MAX_BUFFER];
	OP_TYPE OpType;
}OVER_EX;

typedef struct Client_Info { // 클라이언트 정보 구조체
	mutex Client_mutex;
	SOCKET Socket{};
	OVER_EX Recv_over;
	
	bool IsConnected;
	int User_num; // 간단히 비교할 ID
	int Room_num; // 방접속 유무

	char Packet_buf[MAX_PACKET]; // 패킷을 얼마나 가질 수 있나
	int Prev_data_size; // 이전까지 받은 패킷의 사이즈
	int Cur_packet_size; // 현재 들어온 패킷의 사이즈?

	char Player_ID[MAX_ID_LEN + 1]{}; // 실제 아이디
}CLIENT_INFO;

struct ROOM_EVENT {
	int playerID; // 어떤 플레이어의 
	unsigned char type;		// 무슨 패킷인가
	DWORD data1;
	float fdata;
	// 기타 정보
};

struct Send_Packet_RoomInfo {
	int playerID;
	char buffer[MAX_PACKET];
};