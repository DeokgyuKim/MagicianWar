#pragma once

#include "InterfaceFSM.h"

class Player;
class KeyMgr;
class PlayerFSM : public InterfaceFSM
{
public:
	explicit PlayerFSM(Player* user, BoneType _bone);
	virtual ~PlayerFSM(){}
	PlayerFSM(const PlayerFSM& rhs) = delete;
public:
	// InterfaceCommand��(��) ���� ��ӵ�
	virtual void ChangeState(int _State, int _Ani) override;
	virtual void Enter(int _State, int _Ani) override;
	virtual void Execute(const float& fTimeDelta) override;
	virtual void Exit() override;
private:
	void Idle(const float& fTimeDelta);
	void Move(const float& fTimeDelta);
	void Attack(const float& fTimeDelta);
	void Jump(const float& fTimeDelta);

private:
	Player* m_User;
	KeyMgr* DefaultKeyboard;
	BoneType m_BoneType;
	PLAYER_STATE m_curState;
private:
	bool m_bAttack;
	bool m_bAttackCool;
	float m_fAttackTime;
	float m_fAttackDeltaTime;

	DWORD dkey;
	bool m_bJump;
	bool m_beforejump;
	float m_fJumpTime;
	float m_fJumpDeltaTime;
};

