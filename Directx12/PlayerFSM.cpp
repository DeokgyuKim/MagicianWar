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
void PlayerFSM::Execute(const float& fTimeDelta)
{
	switch (m_State)
	{
		case static_cast<int>(PLAYER_STATE::IDLE) :
			Idle(fTimeDelta);
			break;
		case SCint(PLAYER_STATE::MOVE):
			Move(fTimeDelta);
			break;
		case SCint(PLAYER_STATE::ATTACK):
			Attack(fTimeDelta);
			break;
		case SCint(PLAYER_STATE::JUMP):
			Jump(fTimeDelta);
			break;
	}
}

void PlayerFSM::Idle(const float& fTimeDelta)
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

void PlayerFSM::Move(const float& fTimeDelta)
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

			XMFLOAT3 vForward = dynamic_cast<Transform*>(m_User->GetTransController())->GetForwardVector();
			XMFLOAT3 vBackward = dynamic_cast<Transform*>(m_User->GetTransController())->GetBackwardVector();
			XMFLOAT3 vLeft = dynamic_cast<Transform*>(m_User->GetTransController())->GetLeftVector();
			XMFLOAT3 vRight = dynamic_cast<Transform*>(m_User->GetTransController())->GetRightVector();
			XMFLOAT3 vResult;
			if (DefaultKeyboard->KeyPressing('W'))
			{ // ����
				if (DefaultKeyboard->KeyPressing('A')) {
					// ����
					
#ifndef PHYSX
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveForward(M_MoveForward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveLeft(M_MoveLeft_Speed / sqrtf(2.f));
#else
					XMStoreFloat3(&vResult, XMVector3Normalize(XMLoadFloat3(&vForward) + XMLoadFloat3(&vLeft)) * 5.f);
					m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif

				}
				else if (DefaultKeyboard->KeyPressing('D')) {
					// ������
#ifndef PHYSX
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveForward(M_MoveForward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveRight(M_MoveRight_Speed / sqrtf(2.f));
#else
					XMStoreFloat3(&vResult, XMVector3Normalize(XMLoadFloat3(&vForward) + XMLoadFloat3(&vRight)) * 5.f);
					m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
				}
				else {
#ifndef PHYSX
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveForward(M_MoveForward_Speed);
#else
					XMStoreFloat3(&vResult, XMLoadFloat3(&vForward) * 5.f);
					m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
				}
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKeyboard->KeyPressing('S')) {
				// �Ʒ���

				if (DefaultKeyboard->KeyPressing('A')) {
					// ����
#ifndef PHYSX
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveBackward(M_MoveBackward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveLeft(M_MoveLeft_Speed / sqrtf(2.f));
#else
					XMStoreFloat3(&vResult, XMVector3Normalize(XMLoadFloat3(&vBackward) + XMLoadFloat3(&vLeft)) * 5.f);
					m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
				}
				else if (DefaultKeyboard->KeyPressing('D')) {
					// ������
#ifndef PHYSX
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveBackward(M_MoveBackward_Speed / sqrtf(2.f));
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveRight(M_MoveRight_Speed / sqrtf(2.f));
#else
					XMStoreFloat3(&vResult, XMVector3Normalize(XMLoadFloat3(&vBackward) + XMLoadFloat3(&vRight)) * 5.f);
					m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
				}
				else {
#ifndef PHYSX
					dynamic_cast<Transform*>(m_User->GetTransController())->MoveBackward(M_MoveBackward_Speed);
#else
					XMStoreFloat3(&vResult, XMLoadFloat3(&vBackward) * 5.f);
					m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
				}
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKeyboard->KeyPressing('A')) {
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_LEFT));
#ifndef PHYSX
				dynamic_cast<Transform*>(m_User->GetTransController())->MoveLeft(M_MoveLeft_Speed);
#else
				XMStoreFloat3(&vResult, XMLoadFloat3(&vLeft) * 5.f);
				m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
			}
			else if (DefaultKeyboard->KeyPressing('D')) {
				dynamic_cast<AnimationCom*>(m_User->GetRootAniController())->ChangeAnimation(SCint(ANIMATION_TYPE::WALK_RIGHT));
#ifndef PHYSX
				dynamic_cast<Transform*>(m_User->GetTransController())->MoveRight(M_MoveRight_Speed);
#else
				XMStoreFloat3(&vResult, XMLoadFloat3(&vRight) * 5.f);
				m_User->MoveCapsuleController(vResult, fTimeDelta);
#endif
			}
			else
				ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
	}
}



void PlayerFSM::Attack(const float& fTimeDelta)
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

void PlayerFSM::Jump(const float& fTimeDelta)
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

