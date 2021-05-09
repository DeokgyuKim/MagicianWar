#include "Player.h"
#include "PlayerFSM.h"

PlayerFSM::PlayerFSM(Player* user, BoneType _bone)
	:m_User(user)
{
	m_BoneType = _bone;
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
	dkey = 0;

	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체 애니메이션을 갱신
		m_User->ChangeUpperAnimation(_Ani);
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체 애니메이션을 갱신
		m_User->ChangeRootAnimation(_Ani);
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

void PlayerFSM::SetDefaultKey(DWORD _key)
{
	DefaultKey = _key;
}

void PlayerFSM::Idle()
{
	// 상체 냐 하체냐
	if (m_BoneType == BoneType::UPPER_BONE)
	{ // 상체
		if (DefaultKey & 0x0010) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (DefaultKey & 0x0020) // 마우스 왼쪽 공격
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // 나머지
			if (DefaultKey & 0x0001) // w
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKey & 0x0004) // s
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKey & 0x0002) // a
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKey & 0x0008) // d
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_RIGHT));
			}
		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (DefaultKey & 0x0010) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (DefaultKey & 0x0020) // 마우스 왼쪽 공격
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // 나머지
			if (DefaultKey & 0x0001)
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKey & 0x0004)
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKey & 0x0002)
			{
				ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKey & 0x0008)
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
		if (DefaultKey & 0x0010) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else if (DefaultKey & 0x0020) // 마우스 왼쪽 공격
		{
			ChangeState(static_cast<int>(PLAYER_STATE::ATTACK), SCint(ANIMATION_TYPE::ATTACK));
		}
		else { // 나머지
			if (DefaultKey & 0x0001)
			{
				m_User->ChangeUpperAnimation(SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKey & 0x0004)
			{
				m_User->ChangeUpperAnimation(SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKey & 0x0002)
			{
				m_User->ChangeUpperAnimation(SCint(ANIMATION_TYPE::WALK_LEFT));
			}
			else if (DefaultKey & 0x0008)
			{
				m_User->ChangeUpperAnimation(SCint(ANIMATION_TYPE::WALK_RIGHT));
			}
			else
				ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));

		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (DefaultKey & 0x0010) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
		else { // 이동중에 공격이면 하체는 안바꿔도됨
			if (DefaultKey & 0x0001) // w
			{ // 위쪽
				if (DefaultKey & 0x0002) { // a
					// 왼쪽
					m_User->MoveForward(M_MoveForward_Speed / sqrtf(2.f));
					m_User->MoveLeft(M_MoveLeft_Speed / sqrtf(2.f));
				}
				else if (DefaultKey & 0x0008) { // d
					// 오른쪽
					m_User->MoveForward(M_MoveForward_Speed / sqrtf(2.f));
					m_User->MoveRight(M_MoveRight_Speed / sqrtf(2.f));
				}
				else {
					m_User->MoveForward(M_MoveForward_Speed);
				}
				m_User->ChangeRootAnimation(SCint(ANIMATION_TYPE::WALK_FOWARD));
			}
			else if (DefaultKey & 0x0004) { // s
				// 아래쪽
				if (DefaultKey & 0x0002) { // a
					// 왼쪽
					m_User->MoveBackward(M_MoveBackward_Speed / sqrtf(2.f));
					m_User->MoveLeft(M_MoveLeft_Speed / sqrtf(2.f));
				}
				else if (DefaultKey & 0x0008) { // d
					// 오른쪽
					m_User->MoveBackward(M_MoveBackward_Speed / sqrtf(2.f));
					m_User->MoveRight(M_MoveRight_Speed / sqrtf(2.f));
				}
				else {
					m_User->MoveBackward(M_MoveBackward_Speed);
				}
				m_User->ChangeRootAnimation(SCint(ANIMATION_TYPE::WALK_BACK));
			}
			else if (DefaultKey & 0x0002) { // a
				m_User->ChangeRootAnimation(SCint(ANIMATION_TYPE::WALK_LEFT));
				m_User->MoveLeft(M_MoveLeft_Speed);
			}
			else if (DefaultKey & 0x0008) { // d
				m_User->ChangeRootAnimation(SCint(ANIMATION_TYPE::WALK_RIGHT));
				m_User->MoveRight(M_MoveRight_Speed);
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
		if (m_User->IsAttackEnded()) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
		else if (DefaultKey & 0x0010) // 점프
		{
			ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		}
	}
	else if (m_BoneType == BoneType::ROOT_BONE)
	{ // 하체
		if (m_User->IsAttackEnded()) {
			ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
		}
		else {
			if (DefaultKey & 0x0010) // 점프
			{
				ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
			}
			else { // 이동중에 공격이면 하체는 안바꿔도됨
				if (DefaultKey & 0x0001) // w
				{ // 위쪽
					ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_FOWARD));
				}
				else if (DefaultKey & 0x0004) { // s
					ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_BACK));
				}
				else if (DefaultKey & 0x0002) { // a
					ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_LEFT));
				}
				else if (DefaultKey & 0x0008) { // d
					ChangeState(static_cast<int>(PLAYER_STATE::MOVE), SCint(ANIMATION_TYPE::WALK_RIGHT));
				}
			}
		}
	}
}

void PlayerFSM::Jump()
{
	ChangeState(static_cast<int>(PLAYER_STATE::IDLE), SCint(ANIMATION_TYPE::IDLE));
}
