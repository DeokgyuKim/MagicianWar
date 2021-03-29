#include "AnimationCommand.h"
#include "Animation.h"


AnimationCommand::AnimationCommand(InterfaceAnimation* user)
	: InterfaceCommand(user)
{
}

void AnimationCommand::Execute()
{
	switch (m_State)
	{
		case static_cast<int>(ANIMATION_TYPE::IDLE) :
			Idle();
			break;
		case static_cast<int>(ANIMATION_TYPE::WALK_FOWARD) :
			Move_Forward();
			break;
		case static_cast<int>(ANIMATION_TYPE::WALK_BACK) :
			Move_Back();
			break;
		case static_cast<int>(ANIMATION_TYPE::WALK_LEFT) :
			Move_Left();
			break;
		case static_cast<int>(ANIMATION_TYPE::WALK_RIGHT) :
			Move_Right();
			break;
		case static_cast<int>(ANIMATION_TYPE::ATTACK) :
			Attack();
			break;
		case static_cast<int>(ANIMATION_TYPE::JUMP) :
			Jump();
			break;

	}
}

void AnimationCommand::Idle()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::IDLE);
}

void AnimationCommand::Move_Forward()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::WALK_FOWARD);
}

void AnimationCommand::Move_Back()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::WALK_BACK);
}

void AnimationCommand::Move_Left()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::WALK_LEFT);
}

void AnimationCommand::Move_Right()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::WALK_RIGHT);
}

void AnimationCommand::Attack()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::ATTACK);
}

void AnimationCommand::Jump()
{
	m_User->ChangeAnimation(ANIMATION_TYPE::JUMP);
}
