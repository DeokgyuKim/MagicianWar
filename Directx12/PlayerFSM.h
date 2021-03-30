#pragma once

#include "InterfaceFSM.h"

class Player;
class PlayerFSM : public InterfaceFSM
{
public:
	explicit PlayerFSM(Player* user);
	virtual ~PlayerFSM(){}
	PlayerFSM(const PlayerFSM& rhs) = delete;
public:
	// InterfaceCommand��(��) ���� ��ӵ�
	virtual void SetState(int cEvent) override;
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void Exit() override;
private:
	void Idle();
	void Move();
	void Attack();
	void Jump();





};

