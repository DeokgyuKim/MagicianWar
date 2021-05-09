#pragma once
#include "InterfaceFSM.h"

class Player;

class PlayerFSM : public InterfaceFSM
{
public:
	explicit PlayerFSM(Player* user, BoneType _bone);
	virtual ~PlayerFSM() {}
	PlayerFSM(const PlayerFSM& rhs) = delete;
public:
	// InterfaceCommand을(를) 통해 상속됨
	virtual void ChangeState(int _State, int _Ani) override;
	virtual void Enter(int _State, int _Ani) override;
	virtual void Execute() override;
	virtual void Exit() override;

public:
	void SetDefaultKey(DWORD _key);
private:
	void Idle();
	void Move();
	void Attack();
	void Jump();

private:
	Player* m_User;
	
	BoneType m_BoneType;
	PLAYER_STATE m_curState;
private:
	bool m_bAttack;
	bool m_bAttackCool;
	float m_fAttackTime;
	float m_fAttackDeltaTime;

	DWORD DefaultKey;
	DWORD dkey;
	bool m_bJump;
	bool m_beforejump;
	float m_fJumpTime;
	float m_fJumpDeltaTime;
};