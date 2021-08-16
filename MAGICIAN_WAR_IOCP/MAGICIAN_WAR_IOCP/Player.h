#pragma once

#include "framework.h"
#include "Global.h"
#include "protocol.h"
class InterfaceFSM;


class Player
{
public:
	Player();
	Player(const Player& _rhs);
	Player(int client_num, int room_num);

	Player& operator=(Player& rhs) 
	{
		Initialize(rhs.m_Info.Client_Num, rhs.m_Info.Room_Num);
		setHp(rhs.m_Info.iHp);
		setPosition(rhs.m_Info.xmfPosition);
		setCharacterType(rhs.m_Info.CharacterType);
		setTeam(rhs.m_Info.TeamType);
		return *this;
	}

	~Player();

public:
	void Release();
	void Initialize(int client_num = NO_PLAYER, int room_num = NO_ROOM);
	void InitArray();
	void LateInit();
	void ReInit();
	void Update(float fTime);
	void LateUpdate(float fTime);
	void UpdatePlayerInfo();
	void MoveForward(float speed);
	void MoveBackward(float speed);
	void MoveLeft(float speed);
	void MoveRight(float speed);

	bool Jump(float fTime);

public:
	// Get
	XMFLOAT3 getPosition() { return m_Info.xmfPosition; }
	XMFLOAT3   getRotate() { return m_xmfRotate; }
	XMFLOAT4X4 getWorld() { return m_xmmWorld; }
	XMFLOAT4X4 getDirWorld() { return m_xmmCamDir; }
	XMFLOAT4X4 getBulletStartWorld();
	PlayerInfo getInfo() { return m_Info; }
	int getState(); 
	unsigned char getCharacterType() { return m_Info.CharacterType; }

	int getRootAnimType() { return m_Root_eAnimType; }
	float getRootAnimTime() { return m_Root_fAnimTime; }
	int getRootAnimBlendType() { return m_Root_eAnimBlendType; }
	float getRootAnimWeight() { return m_Root_fWeight; }

	int getUpperAnimType() { return m_Upper_eAnimType; }
	float getUpperAnimTime() { return m_Upper_fAnimTime; }
	int getUpperAnimBlendType() { return m_Upper_eAnimBlendType; }
	float getUpperAnimWeight() { return m_Upper_fWeight; }
	
	DWORD getKey() const { return m_keyInput; } // 눌린 키?
	int getHp() { return m_Info.iHp; }

	bool IsAttackEnded() { return m_bAttackEnd; }
	int getID() { return m_Info.Client_Num; }
	bool getHost() { return m_Info.isRoom_Host; }
	bool getReady() { return m_Ready; }
	char getTeam() { return m_Info.TeamType; }
	int getSlotNum() { return m_slot_num; }
	Camera getCamera() { return m_Camera; }
	bool getUsed() { return m_Used; }
	bool getAbleHeal();
	bool getAbleDottAtt();
	bool getAbleDealing();
	bool getFreeze();

	InterfaceFSM* GetRootFSM();
	InterfaceFSM* GetUpperFSM();

	// Set
	void setPlayerInfo(PlayerInfo _info) { m_Info = _info; }
	void setID(int _id) { m_Info.Client_Num = _id; }
	void setHp(int iHp) { m_Info.iHp = iHp; }
	int setDamage(int damage,int type = BULLET_HIT_EVENT);
	void setReady(bool _ready) { m_Ready = _ready; }
	void setTeam(char _team) { m_Info.TeamType = _team; }
	

	void setKeyInput(DWORD _key);
	void setPosition(XMFLOAT3 pos);
	void setAttackEnd(bool _check) { m_bAttackEnd = _check; }
	void setPlayerInitPos(XMFLOAT3 pos);

	void setCreateBullet(int count) { m_Bullet = count; }
	int getCreateBullet() { return m_Bullet; }
	void ChangeUpperAnimation(int _Ani);
	void ChangeRootAnimation(int _Ani);
	void setJump(bool bJump);
	void setCharacterType(unsigned char _type);
	void setHost(bool bHost);
	void setSlotNum(int num) { m_slot_num = num; }
	void setCamera(float x, float y) { m_Camera.CamX = x; m_Camera.CamY = y; }
	void setUsed(bool _use) { m_Used = _use; }

public:
	void SetScale(XMFLOAT3 xmfSclae) { m_xmfScale = xmfSclae; }
	void SetRotate(XMFLOAT3 xmfRotate);

	bool CanDisconnect() const { return m_CanDisconnect; }
	void SetCanDisconnect(bool _bCan) { m_CanDisconnect = _bCan; }
public:
	//PhysX
	void CreateCapsuleController();
	void ModifyPhysXPos(const float& fTimeDelta);
	void GravityProgress(const float& fTimeDelta);
	void MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta);
	PxCapsuleController* GetPxCapsuleController() { return m_pCapsuleCon; }

private: // 
	// info
	PlayerInfo	m_Info;
	Camera		m_Camera;
	int m_slot_num;
	bool m_Ready;
	bool m_Room_JoinState;
	bool m_LateInit;
	float m_fHpHealTime = 0.f;
	float m_fHpDealTime = 0.f;

	bool m_Used = false;

	// 초기화
	XMFLOAT4X4		m_matRealWorld;
	XMFLOAT3		m_xmfScale;
	XMFLOAT3		m_xmfRotate;
	XMFLOAT3		m_xmfMeshRotate;
	XMFLOAT4X4		m_xmmWorld;
	XMFLOAT4X4		m_xmmCamDir;


	// 하체 ( 기준 )
	int				m_Root_eAnimType;
	float			m_Root_fAnimTime;
	int				m_Root_eAnimBlendType;
	float			m_Root_fWeight;

	// 상체
	int				m_Upper_eAnimType;
	float			m_Upper_fAnimTime;
	int				m_Upper_eAnimBlendType;
	float			m_Upper_fWeight;

	DWORD			m_keyInput;

	bool m_bAttackEnd;
	// FSM
	unique_ptr<InterfaceFSM>                    m_UpperBody;    // 상체
	unique_ptr<InterfaceFSM>                    m_RootBody;     // 하체

	//PhysX
	PxCapsuleController* m_pCapsuleCon = nullptr;

	int m_Bullet;

	bool m_CanDisconnect;

	//Jump
	bool m_bJump = false;
	float m_fJumpSpeedY = 0.f;
	float m_fAccTime = 0.f;
};

