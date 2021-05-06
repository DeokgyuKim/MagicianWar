#pragma once

#include <WS2tcpip.h>
#include <mutex>
#include "include.h"


class Player
{
public:
	Player();
	~Player();

	void Initialize(SOCKET& sock, int ID);
	void Update();
	void UpdatePlayerInfo(CTOS_PlayerInfo* pInfo);
	void UpdatePosition();
	void noTransWorldUpdate(XMFLOAT4X4 _world);
	void MoveForward(float speed);
	void MoveBackward(float speed);
	void MoveLeft(float speed);
	void MoveRight(float speed);

	// recv & send
	int recvPacket();
	int sendPacket(void* _packet, int _size);

	// mutex
	void Lock() { m_mutex.lock(); }
	void Unlock() { m_mutex.unlock(); }

public:
	// Get
	XMFLOAT4X4 getWorld() { return matWorld; }
	STOC_ServerPlayer getInfo() { return Info; }
	SOCKET getSocket() { return Info.socket; }
	PLAYER_STATE getState() { return ePlayerState; }
	ANIMATION_TYPE getAnimType() { return eAnimType; }
	float getAnimTime() { return fAnimTime; }
	ANIMATION_TYPE getAnimBlendType() { return eAnimBlendType; }
	float getAnimWeight() { return fWeight; }
	bool getReady() { return m_Ready; }
	char* getBuffer() { return recvBuf; }
	char* getPacketStart_Ptr() { return packet_start_ptr; }
	char* getRecvStart_Ptr() { return recv_start_ptr; }
	bool IsConnected() { return m_isConnected; }

	// Set
	void setPlayerInfo(PlayerInfo _info) { Info.info = _info; }
	void setPacketStart_Ptr(char* _ptr) { packet_start_ptr = _ptr; }
	void setRecvStart_Ptr(char* _ptr) { recv_start_ptr = _ptr; };
	void resetPacket_Ptr() { packet_start_ptr = recv_start_ptr; }
	void setKeyInput(DWORD _key);
	void setReady(DWORD _ready);
private: // 
	// info
	STOC_ServerPlayer Info;
	XMFLOAT4X4		matWorld;
	PLAYER_STATE	ePlayerState;
	ANIMATION_TYPE	eAnimType;
	float			fAnimTime;
	ANIMATION_TYPE	eAnimBlendType;
	float			fWeight;


	mutex m_mutex;
	bool m_Ready; // ready
	bool m_isConnected;

	char recvBuf[MAX_BUFFER]; // buffer
	char* recv_start_ptr;
	char* packet_start_ptr;

	DWORD m_keyInput;
};
