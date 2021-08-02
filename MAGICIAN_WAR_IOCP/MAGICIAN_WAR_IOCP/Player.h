#pragma once

#include "framework.h"
#include "extern.h"
#include "protocol.h"
class InterfaceFSM;

class Player
{
public:
	Player();
	~Player();

public:
	void Initialize();
	void ReInit();
	void Update(float fTime);
	void UpdatePlayerInfo(PlayerInfo* pInfo);
	void UpdatePosition();
	void noTransWorldUpdate(XMFLOAT4X4 _world);
	void MoveForward(float speed);
	void MoveBackward(float speed);
	void MoveLeft(float speed);
	void MoveRight(float speed);

	bool Jump(float fTime);

public:
	// Get
	XMFLOAT3 getPosition() { return XMFLOAT3(m_matRealWorld._41, m_matRealWorld._42, m_matRealWorld._43); }
	XMFLOAT4X4 getWorld() { return m_Info.matWorld; }
	XMFLOAT4X4 getBulletStartWorld();
	PlayerInfo getInfo() { return m_Info; }
	int getState() { return m_Info.PlayerState; }
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
	bool getReady() { return m_Ready; }

	InterfaceFSM* GetRootFSM();
	InterfaceFSM* GetUpperFSM();

	// Set
	void setPlayerInfo(PlayerInfo _info) { m_Info = _info; }
	void setPlayerHp(UINT iHp) { m_Info.iHp = iHp; }
	void setReady(bool _ready) { m_Ready = _ready; }

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

public:
	//PhysX
	void CreateCapsuleController();
	void ModifyPhysXPos(const float& fTimeDelta);
	void GravityProgress(const float& fTimeDelta);
	void MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta);
	//PxCapsuleController* GetPxCapsuleController() { return m_pCapsuleCon; }

private: // 
	// info
	PlayerInfo m_Info;
	bool m_Ready;
	bool m_Room_JoinState;

	XMFLOAT4X4		m_matRealWorld;

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
	//PxCapsuleController* m_pCapsuleCon = nullptr;

	int m_Bullet;

	//Jump
	bool m_bJump = false;
	float m_fJumpSpeedY = 0.f;
	float m_fAccTime = 0.f;
};

