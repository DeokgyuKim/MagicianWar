#pragma once
#include "InterfaceFSM.h"

class Player;

class PlayerFSM :
    public InterfaceFSM
{
public:
	explicit PlayerFSM(Player* user, int _bone);
	virtual ~PlayerFSM() {}
	PlayerFSM(const PlayerFSM& rhs) = delete;
public:
	// InterfaceCommand��(��) ���� ��ӵ�
	virtual void ChangeState(int _State, int _Ani) override;
	virtual void Enter(int _State, int _Ani) override;
	virtual void Execute(float fTime) override;
	virtual void Exit() override;

public:
	void SetDefaultKey(int _key);
	//bool GetAttackEnd() { return m_bAttack; }
private:
	void Idle(float fTime);
	void Move(float fTime);
	void Attack(float fTime);
	void Jump(float fTime);
	void Hit(float fTime);
	void Dance(float fTime);
	void Dead(float fTime);
	void Freeze(float fTime);

private:
	Player* m_User;

	int m_BoneType; // �� Ÿ��
	int m_curState; // ���� �÷��̾� ����
private:
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
	float m_fFreezeTime;
};