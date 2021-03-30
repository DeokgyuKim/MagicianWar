#include "PlayerFSM.h"
#include "Player.h"


PlayerFSM::PlayerFSM(Player* user)
	: InterfaceFSM(user)
{
}

void PlayerFSM::SetState(int cEvent)
{

}

void PlayerFSM::Enter()
{
}

void PlayerFSM::Exit()
{
}
void PlayerFSM::Execute()
{
	switch (m_State)
	{
		case static_cast<int>(PLAYER_STATE::IDLE) :
			Idle();
			break;
		case static_cast<int>(PLAYER_STATE::MOVE) :
			Move();
			break;
		case static_cast<int>(PLAYER_STATE::ATTACK) :
			Attack();
			break;
		case static_cast<int>(PLAYER_STATE::JUMP) :
			Jump();
			break;
	}
}

void PlayerFSM::Idle()
{
	//
}

void PlayerFSM::Move()
{
	//
}



void PlayerFSM::Attack()
{
	//m_User->ChangeAnimation(ANIMATION_TYPE::ATTACK);
}

void PlayerFSM::Jump()
{
	//m_User->ChangeAnimation(ANIMATION_TYPE::JUMP);
}

