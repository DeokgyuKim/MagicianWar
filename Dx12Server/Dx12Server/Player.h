#pragma once

#include <mutex>
#include "include.h"

class InterfaceFSM;

class Player
{
public:
	Player();
	~Player();

	void Initialize(SOCKET& sock, int ID);
	void Update(float fTime);
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
	XMFLOAT4X4 getBulletStartWorld();
	STOC_ServerPlayer getInfo() { return Info; }
	SOCKET getSocket() { return Info.socket; }
	PLAYER_STATE getState() { return ePlayerState; }

	ANIMATION_TYPE getRootAnimType() { return Root_eAnimType; }
	float getRootAnimTime() { return Root_fAnimTime; }
	ANIMATION_TYPE getRootAnimBlendType() { return Root_eAnimBlendType; }
	float getRootAnimWeight() { return Root_fWeight; }

	ANIMATION_TYPE getUpperAnimType() { return Upper_eAnimType; }
	float getUpperAnimTime() { return Upper_fAnimTime; }
	ANIMATION_TYPE getUpperAnimBlendType() { return Upper_eAnimBlendType; }
	float getUpperAnimWeight() { return Upper_fWeight; }
	DWORD getKey() const { return m_keyInput; }
	int getHp() { return Info.info.iHp; }
	
	bool getReady() { return m_Ready; }
	char* getBuffer() { return recvBuf; }
	char* getPacketStart_Ptr() { return packet_start_ptr; }
	char* getRecvStart_Ptr() { return recv_start_ptr; }
	bool IsConnected() { return m_isConnected; }
	bool IsAttackEnded() { return m_bAttackEnd; }
	bool getLoaddingEnd() { return m_LoadingEnd; }
	unsigned char getInstanceName() { return m_InstanceName; }
	unsigned char getID() { return m_ID; }

	InterfaceFSM* GetRootFSM();
	InterfaceFSM* GetUpperFSM();

	// Set
	void setPlayerInfo(PlayerInfo _info) { Info.info = _info; }
	void setPlayerHp(UINT iHp) { Info.info.iHp = iHp; }
	void setPacketStart_Ptr(char* _ptr) { packet_start_ptr = _ptr; }
	void setRecvStart_Ptr(char* _ptr) { recv_start_ptr = _ptr; };
	void resetPacket_Ptr() { packet_start_ptr = recv_start_ptr; }
	void setKeyInput(DWORD _key);
	void setReady(DWORD _ready);
	void setPosition(XMFLOAT3 pos);
	void setAttackEnd(bool _check) { m_bAttackEnd = _check; }
	void setPlayerInitPos(XMFLOAT3 pos);
	void setLoaddingEnd(bool _load);
	

	void ChangeUpperAnimation(int _Ani);
	void ChangeRootAnimation(int _Ani);

public:
	//PhysX
	void CreateCapsuleController();
	void ModifyPhysXPos(const float& fTimeDelta);
	void GravityProgress(const float& fTimeDelta);
	void MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta);
	PxCapsuleController* GetPxCapsuleController() { return m_pCapsuleCon; }

private: // 
	// info
	STOC_ServerPlayer Info;
	XMFLOAT4X4		matWorld;
	XMFLOAT4X4		m_matRealWorld;
	PLAYER_STATE	ePlayerState;

	ANIMATION_TYPE	Root_eAnimType;
	float			Root_fAnimTime;
	ANIMATION_TYPE	Root_eAnimBlendType;
	float			Root_fWeight;

	ANIMATION_TYPE	Upper_eAnimType;
	float			Upper_fAnimTime;
	ANIMATION_TYPE	Upper_eAnimBlendType;
	float			Upper_fWeight;


	mutex m_mutex;
	bool m_Ready; // ready
	bool m_LoadingEnd;
	bool m_isConnected;


	char recvBuf[MAX_BUFFER]; // buffer
	char* recv_start_ptr;
	char* packet_start_ptr;

	DWORD m_keyInput;

	bool m_bAttackEnd;

	// 05 11 bullet
	unsigned char m_InstanceName;
	unsigned char m_ID;

	// FSM
	unique_ptr<InterfaceFSM>                    m_UpperBody;    // 상체
	unique_ptr<InterfaceFSM>                    m_RootBody;     // 하체

	//PhysX
	PxCapsuleController* m_pCapsuleCon = nullptr;

	int m_iHp;
};

