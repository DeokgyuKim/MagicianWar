#include "PlayerFSM.h"
#include "Player.h"
#include "Animation.h"
#include "Transform.h"
#include "KeyMgr.h"


PlayerFSM::PlayerFSM(Player* user, BoneType _bone)
	: m_User(user)
{
	m_BoneType = _bone;
	DefaultKeyboard = KeyMgr::GetInstance();
	m_curState = PLAYER_STATE::IDLE;
}

void PlayerFSM::ChangeState(int _State, int _Ani)
{
	// 플레이어 상태 가 동일하면 냅둠
	if (m_State == _State) {
		return;
	}

	Exit();
	
	m_State = _State;
	
	Enter(_State, _Ani);

}

void PlayerFSM::Enter(int _State, int _Ani)
{

	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체 애니메이션을 갱신
		dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->ChangeAnimation(_Ani);
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체 애니메이션을 갱신
		dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(_Ani);
	}

	switch (m_State)
	{
		case static_cast<int>(PLAYER_STATE::IDLE) :

			break;
		case SCint(PLAYER_STATE::MOVE):

			break;
		case SCint(PLAYER_STATE::ATTACK):

			break;
		case SCint(PLAYER_STATE::JUMP):

			break;
	}
}

void PlayerFSM::Exit()
{
	// 상태 변환 하기 전에 이전 상태에서 마무리 할 것
	switch (m_State)
	{
		case static_cast<int>(PLAYER_STATE::IDLE) :

			break;
		case SCint(PLAYER_STATE::MOVE):

			break;
		case SCint(PLAYER_STATE::ATTACK):

			break;
		case SCint(PLAYER_STATE::JUMP):

			break;
	}
}
void PlayerFSM::Execute()
{
	switch (m_State)
	{
		case static_cast<int>(PLAYER_STATE::IDLE) :
			Idle();
			break;
		case SCint(PLAYER_STATE::MOVE):
			Move();
			break;
		case SCint(PLAYER_STATE::ATTACK):
			Attack();
			break;
		case SCint(PLAYER_STATE::JUMP):
			Jump();
			break;
	}
}

void PlayerFSM::Idle()
{
	// 상체 냐 하체냐
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // 마우스 왼쪽 공격
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // 나머지
			if (DefaultKeyboard->KeyPressing('W'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKeyboard->KeyPressing('S'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKeyboard->KeyPressing('A'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKeyboard->KeyPressing('D'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_RIGHT));
			}
		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // 마우스 왼쪽 공격
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // 나머지
			if (DefaultKeyboard->KeyPressing('W'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKeyboard->KeyPressing('S'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKeyboard->KeyPressing('A'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKeyboard->KeyPressing('D'))
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_RIGHT));
			}
		}
	}
}

void PlayerFSM::Move()
{
	//
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // 마우스 왼쪽 공격
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // 나머지
			if (DefaultKeyboard->KeyPressing('W'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKeyboard->KeyPressing('S'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKeyboard->KeyPressing('A'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKeyboard->KeyPressing('D'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_RIGHT));
			}
			else
				ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else { // 이동중에 공격이면 하체는 안바꿔도됨
			if (DefaultKeyboard->KeyPressing('W'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKeyboard->KeyPressing('S'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKeyboard->KeyPressing('A'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKeyboard->KeyPressing('D'))
			{
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_RIGHT));
			}
			else
				ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
}



void PlayerFSM::Attack()
{
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체
		if (dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->GetAttackEnd()) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->GetAttackEnd()) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
}

void PlayerFSM::Jump()
{
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체
		// 상체나 하체나 User가 땅에 닿았는가를 체크하는게 정배인듯?
		if (dynamic_cast<Transform*>(m_User->GetTransController())->GetPosition().y <= 0.f) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
		else
		{

		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (dynamic_cast<Transform*>(m_User->GetTransController())->GetPosition().y <= 0.f) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
		else
		{

		}
	}
}

