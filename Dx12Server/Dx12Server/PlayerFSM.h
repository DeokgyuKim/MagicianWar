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
	// InterfaceCommand��(��) ���� ��ӵ�
	virtual void ChangeState(int _State, int _Ani) override;
	virtual void Enter(int _State, int _Ani) override;
	virtual void Execute(float fTime) override;
	virtual void Exit() override;

public:
	void SetDefaultKey(DWORD _key);
private:
	void Idle(float fTime);
	void Move(float fTime);
	void Attack(float fTime);
	void Jump(float fTime);
	void Hit(float fTime);

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

	float m_AnimTime;
	float m_fHitTime;
};