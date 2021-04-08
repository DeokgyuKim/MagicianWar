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
	// �÷��̾� ���� �� �����ϸ� ����
	if (m_State == _State) {
		return;
	}

	Exit();

	m_State = _State;

	Enter(_State, _Ani);

}

void PlayerFSM::Enter(int _State, int _Ani)
{
	dkey = 0;

	if (m_BoneType == BoneType::UPPER_BONE)
	{ // ��ü �ִϸ��̼��� ����
		dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->ChangeAnimation(_Ani);
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // ��ü �ִϸ��̼��� ����
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
			m_beforejump = false;
			break;
	}
}

void PlayerFSM::Exit()
{
	// ���� ��ȯ �ϱ� ���� ���� ���¿��� ������ �� ��
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
	// ��ü �� ��ü��
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // ��ü
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // ���콺 ���� ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // ������
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
	{ // ��ü
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // ���콺 ���� ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // ������
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
	{ // ��ü
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) // ���콺 ���� ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // ������
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
	{ // ��ü
		if (DefaultKeyboard->KeyPressing(VK_SPACE)) // ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else { // �̵��߿� �����̸� ��ü�� �ȹٲ㵵��
			if (DefaultKeyboard->KeyPressing('W'))
			{ // ����
				if (DefaultKeyboard->KeyPressing('A')) {
					// ����
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveForward(M_MoveForward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveLeft(M_MoveLeft_Speed / sqrtf(2.f));
				}
				else if (DefaultKeyboard->KeyPressing('D')) {
					// ������
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveForward(M_MoveForward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveRight(M_MoveRight_Speed / sqrtf(2.f));
				}
				else {
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveForward(M_MoveForward_Speed);
				}
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKeyboard->KeyPressing('S')) {
				// �Ʒ���
				if (DefaultKeyboard->KeyPressing('A')) {
					// ����
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveBackward(M_MoveBackward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveLeft(M_MoveLeft_Speed / sqrtf(2.f));
				}
				else if (DefaultKeyboard->KeyPressing('D')) {
					// ������
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveBackward(M_MoveBackward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveRight(M_MoveRight_Speed / sqrtf(2.f));
				}
				else {
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveBackward(M_MoveBackward_Speed);
				}
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKeyboard->KeyPressing('A')) {
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_LEFT));
				//dynamic_cast<Transform*>(m_User->GetTransController())->MoveLeft(M_MoveLeft_Speed);
			}
			else if (DefaultKeyboard->KeyPressing('D')) {
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_RIGHT));
				//dynamic_cast<Transform*>(m_User->GetTransController())->MoveRight(M_MoveRight_Speed);
			}
			else
				ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
}



void PlayerFSM::Attack()
{
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // ��ü
		if (dynamic_cast<AnimationCom*>(m_User->GetUpperAniController())->GetAttackEnd()) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
		else if (DefaultKeyboard->KeyPressing(VK_SPACE)) // ����
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // ��ü
		if (dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->GetAttackEnd()) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
}

void PlayerFSM::Jump()
{
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // ��ü
		// ��ü�� ��ü�� User�� ���� ��Ҵ°��� üũ�ϴ°� �����ε�?
		if (m_User->GetPosition().y <= 0.f) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}

	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // ��ü
		if (!m_beforejump) {
			if (DefaultKeyboard->KeyPressing('W')) dkey |= KEY_W;
			if (DefaultKeyboard->KeyPressing('S')) dkey |= KEY_S;
			if (DefaultKeyboard->KeyPressing('A')) dkey |= KEY_A;
			if (DefaultKeyboard->KeyPressing('D')) dkey |= KEY_D;
			m_beforejump = true;
		}

		if (dynamic_cast<Transform*>(m_User->GetTransController())->Jump(dkey)) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
		else
		{

		}
	}
}

