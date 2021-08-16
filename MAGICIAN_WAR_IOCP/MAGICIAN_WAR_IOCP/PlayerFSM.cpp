#include "Player.h"
#include "PlayerFSM.h"

PlayerFSM::PlayerFSM(Player* user, int _bone)
	:m_User(user)
{
	m_BoneType = _bone;
	m_curState = STATE_IDLE;

	m_fHitTime = 1.0f;
	m_fAttackTime = 1.f;
	m_fFreezeTime = 5.f;
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
	m_AnimTime = 0.f;
	//m_bAttack = false;

	if (m_BoneType == BONE_UPPER)
	{ // ��ü �ִϸ��̼��� ����
		m_User->ChangeUpperAnimation(_Ani);

	}
	else if (m_BoneType == BONE_ROOT)
	{ // ��ü �ִϸ��̼��� ����
		m_User->ChangeRootAnimation(_Ani);
	}

	switch (m_State)
	{
		case STATE_IDLE :

			break;
		case STATE_MOVE:

			break;
		case STATE_ATTACK:

			break;
		case STATE_JUMP:
			m_beforejump = false;
			m_User->setJump(true);
			break;
		case STATE_HIT:

			break;
		case STATE_DANCE:

			break;
		case STATE_DEAD:

			break;
	}
}

void PlayerFSM::Execute(float fTime)
{
	m_AnimTime += fTime;
	switch (m_State)
	{
		case STATE_IDLE:
			Idle(fTime);
			break;
		case STATE_MOVE:
			Move(fTime);
			break;
		case STATE_ATTACK:
			Attack(fTime);
			break;
		case STATE_JUMP:
			Jump(fTime);
			break;
		case STATE_HIT:
			Hit(fTime);
			break;
		case STATE_DANCE:
			Dance(fTime);
			break;
		case STATE_FREEZE:
			Freeze(fTime);
			break;
		case STATE_DEAD:
			Dead(fTime);
			break;
	}
}

void PlayerFSM::Exit()
{
	// ���� ��ȯ �ϱ� ���� ���� ���¿��� ������ �� ��
	switch (m_State)
	{
		case STATE_IDLE:

			break;
		case STATE_MOVE:

			break;
		case STATE_ATTACK:
			
			break;
		case STATE_JUMP:
			m_User->setJump(false);
			break;
		case STATE_HIT:

			break;
		case STATE_DANCE:

			break;
		case STATE_DEAD:

			break;
	}
}

void PlayerFSM::SetDefaultKey(int _key)
{
	DefaultKey = _key;
}

void PlayerFSM::Idle(float fTime)
{
	// ��ü �� ��ü��
	if (m_BoneType == BONE_UPPER)
	{ // ��ü
		//NoJump
		//if (DefaultKey & 0x0010) // ����
		//{
		//	ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		//	dkey = DefaultKey;
		//}
		//else
		if (DefaultKey & ctos_KEY_LBUTTON) // ���콺 ���� ����
		{
			ChangeState(STATE_ATTACK, ANIM_ATTACK);
		}
		else { // ������
			if (DefaultKey & ctos_KEY_W) // w
			{
				ChangeState(STATE_MOVE, ANIM_WALK_FOWARD);
			}
			else if (DefaultKey & ctos_KEY_S) // s
			{
				ChangeState(STATE_MOVE, ANIM_WALK_BACK);
			}
			else if (DefaultKey & ctos_KEY_A) // a
			{
				ChangeState(STATE_MOVE, ANIM_WALK_LEFT);
			}
			else if (DefaultKey & ctos_KEY_D) // d
			{
				ChangeState(STATE_MOVE, ANIM_WALK_RIGHT);
			}
		}
	}
	else if (m_BoneType == BONE_ROOT)
	{ // ��ü
		//NoJump
		//if (DefaultKey & 0x0010) // ����
		//{
		//	ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		//	dkey = DefaultKey;
		//}
		//else
		if (DefaultKey & ctos_KEY_LBUTTON) // ���콺 ���� ����
		{
			ChangeState(STATE_ATTACK, ANIM_ATTACK);
		}
		else { // ������
			if (DefaultKey & ctos_KEY_W)
			{
				ChangeState(STATE_MOVE, ANIM_WALK_FOWARD);
			}
			else if (DefaultKey & ctos_KEY_S)
			{
				ChangeState(STATE_MOVE, ANIM_WALK_BACK);
			}
			else if (DefaultKey & ctos_KEY_A)
			{
				ChangeState(STATE_MOVE, ANIM_WALK_LEFT);
			}
			else if (DefaultKey & ctos_KEY_D)
			{
				ChangeState(STATE_MOVE, ANIM_WALK_RIGHT);
			}
		}
	}
}

void PlayerFSM::Move(float fTime)
{
	//
	if (m_BoneType == BONE_UPPER)
	{ // ��ü
		//NoJump
		//if (DefaultKey & 0x0010) // ����
		//{
		//	ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		//	dkey = DefaultKey;
		//}
		//else
		if (DefaultKey & ctos_KEY_LBUTTON) // ���콺 ���� ����
		{
			ChangeState(STATE_ATTACK, ANIM_ATTACK);
		}
		else { // ������
			if (DefaultKey & ctos_KEY_W)
			{
				m_User->ChangeUpperAnimation(ANIM_WALK_FOWARD);
			}
			else if (DefaultKey & ctos_KEY_S)
			{
				m_User->ChangeUpperAnimation(ANIM_WALK_BACK);
			}
			else if (DefaultKey & ctos_KEY_A)
			{
				m_User->ChangeUpperAnimation(ANIM_WALK_LEFT);
			}
			else if (DefaultKey & ctos_KEY_D)
			{
				m_User->ChangeUpperAnimation(ANIM_WALK_RIGHT);
			}
			else
				ChangeState(STATE_IDLE, ANIM_IDLE);

		}
	}
	else if (m_BoneType == BONE_ROOT)
	{ // ��ü
		//NoJump
		//if (DefaultKey & 0x0010) // ����
		//{
		//	ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		//	dkey = DefaultKey;
		//}
		//else
		{ // �̵��߿� �����̸� ��ü�� �ȹٲ㵵��
			if (DefaultKey & ctos_KEY_W) // w
			{ // ����
				if (DefaultKey & ctos_KEY_A) { // a
					// ����
					m_User->MoveForward(P_MoveForward_Speed / sqrtf(2.f)* fTime);
					m_User->MoveLeft(P_MoveLeft_Speed / sqrtf(2.f)* fTime);
				}
				else if (DefaultKey & ctos_KEY_D) { // d
					// ������
					m_User->MoveForward(P_MoveForward_Speed / sqrtf(2.f)* fTime);
					m_User->MoveRight(P_MoveRight_Speed / sqrtf(2.f)* fTime);
				}
				else {
					m_User->MoveForward(P_MoveForward_Speed* fTime);
				}
				m_User->ChangeRootAnimation(ANIM_WALK_FOWARD);
			}
			else if (DefaultKey & ctos_KEY_S) { // s
				// �Ʒ���
				if (DefaultKey & ctos_KEY_A) { // a
					// ����
					m_User->MoveBackward(P_MoveBackward_Speed / sqrtf(2.f)* fTime);
					m_User->MoveLeft(P_MoveLeft_Speed / sqrtf(2.f)* fTime);
				}
				else if (DefaultKey & ctos_KEY_D) { // d
					// ������
					m_User->MoveBackward(P_MoveBackward_Speed / sqrtf(2.f)* fTime);
					m_User->MoveRight(P_MoveRight_Speed / sqrtf(2.f)* fTime);
				}
				else {
					m_User->MoveBackward(P_MoveBackward_Speed* fTime);
				}
				m_User->ChangeRootAnimation(ANIM_WALK_BACK);
			}
			else if (DefaultKey & ctos_KEY_A) { // a
				m_User->ChangeRootAnimation(ANIM_WALK_LEFT);
				m_User->MoveLeft(P_MoveLeft_Speed* fTime);
			}
			else if (DefaultKey & ctos_KEY_D) { // d
				m_User->ChangeRootAnimation(ANIM_WALK_RIGHT);
				m_User->MoveRight(P_MoveRight_Speed* fTime);
			}
			else
				ChangeState(STATE_IDLE, ANIM_IDLE);
		}
	}
}

void PlayerFSM::Attack(float fTime)
{
	if (m_BoneType == BONE_UPPER)
	{ // ��ü
		if (m_User->IsAttackEnded()) {
			ChangeState(STATE_IDLE, ANIM_IDLE);
		
		}
		
		//NoJump
		//else if (DefaultKey & 0x0010) // ����
		//{
		//	ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
		//}
	}
	else if (m_BoneType == BONE_ROOT)
	{ // ��ü
		if (m_User->IsAttackEnded()) {
			ChangeState(STATE_IDLE, ANIM_IDLE);
		}
		else {
			//NoJump
			//if (DefaultKey & 0x0010) // ����
			//{
			//	ChangeState(static_cast<int>(PLAYER_STATE::JUMP), SCint(ANIMATION_TYPE::JUMP));
			//}
			//else 
			{ // �̵��߿� �����̸� ��ü�� �ȹٲ㵵��
				if (DefaultKey & ctos_KEY_W) // w
				{ // ����
					ChangeState(STATE_MOVE, ANIM_WALK_FOWARD);
				}
				else if (DefaultKey & ctos_KEY_S) { // s
					ChangeState(STATE_MOVE, ANIM_WALK_BACK);
				}
				else if (DefaultKey & ctos_KEY_A) { // a
					ChangeState(STATE_MOVE, ANIM_WALK_LEFT);
				}
				else if (DefaultKey & ctos_KEY_D) { // d
					ChangeState(STATE_MOVE, ANIM_WALK_RIGHT);
				}
			}
		}
	}
}

void PlayerFSM::Jump(float fTime)
{
	if (m_BoneType == BONE_UPPER)
	{
		if (m_User->getPosition().y <= 0.f) {
			ChangeState(STATE_IDLE, ANIM_IDLE);
		}

	}
	else if (m_BoneType == BONE_ROOT)
	{
		if (dkey & ctos_KEY_W) // w
		{ // ����
			if (dkey & ctos_KEY_A) { // a
				// ����
				m_User->MoveForward(P_MoveForward_Speed / sqrtf(2.f)* fTime);
				m_User->MoveLeft(P_MoveLeft_Speed / sqrtf(2.f)* fTime);
			}
			else if (dkey & ctos_KEY_D) { // d
				// ������
				m_User->MoveForward(P_MoveForward_Speed / sqrtf(2.f)* fTime);
				m_User->MoveRight(P_MoveRight_Speed / sqrtf(2.f)* fTime);
			}
			else {
				m_User->MoveForward(P_MoveForward_Speed* fTime);
			}
			m_User->ChangeRootAnimation(ANIM_WALK_FOWARD);
		}
		else if (dkey & ctos_KEY_S) { // s
			// �Ʒ���
			if (dkey & ctos_KEY_A) { // a
				// ����
				m_User->MoveBackward(P_MoveBackward_Speed / sqrtf(2.f)* fTime);
				m_User->MoveLeft(P_MoveLeft_Speed / sqrtf(2.f)* fTime);
			}
			else if (dkey & ctos_KEY_D) { // d
				// ������
				m_User->MoveBackward(P_MoveBackward_Speed / sqrtf(2.f)* fTime);
				m_User->MoveRight(P_MoveRight_Speed / sqrtf(2.f)* fTime);
			}
			else {
				m_User->MoveBackward(P_MoveBackward_Speed*fTime);
			}
			m_User->ChangeRootAnimation(ANIM_WALK_BACK);
		}
		else if (dkey & ctos_KEY_A) { // a
			m_User->ChangeRootAnimation(ANIM_WALK_LEFT);
			m_User->MoveLeft(P_MoveLeft_Speed * fTime);
		}
		else if (dkey & ctos_KEY_D) { // d
			m_User->ChangeRootAnimation(ANIM_WALK_RIGHT);
			m_User->MoveRight(P_MoveRight_Speed * fTime);
		}

		if (m_User->Jump(fTime))
			ChangeState(STATE_IDLE, ANIM_IDLE);

		if (m_User->getPosition().y <= 0.f) {
			ChangeState(STATE_IDLE, ANIM_IDLE);
		}
	}
}

void PlayerFSM::Hit(float fTime)
{
	if (m_BoneType == BONE_UPPER)
	{ // ��ü
		if (m_AnimTime >= m_fHitTime) { // �ð��� �ٵǸ�
			ChangeState(STATE_IDLE, ANIM_IDLE);

		}
	}
	else if (m_BoneType == BONE_ROOT)
	{ // ��ü
		ChangeState(STATE_IDLE, ANIM_IDLE);
	}
}

void PlayerFSM::Dance(float fTime)
{
	if (m_BoneType == BONE_UPPER)
	{ // ��ü

	}
	else if (m_BoneType == BONE_ROOT)
	{

	}
}

void PlayerFSM::Dead(float fTime)
{
	if (m_BoneType == BONE_UPPER)
	{ // ��ü

	}
	else if (m_BoneType == BONE_ROOT)
	{

	}
}

void PlayerFSM::Freeze(float fTime)
{
	if (m_BoneType == BONE_UPPER)
	{ // ��ü
		if (m_fFreezeTime >= m_AnimTime) {
			ChangeState(STATE_IDLE, ANIM_IDLE);
		}
	}
	else if (m_BoneType == BONE_ROOT)
	{
		if (m_fFreezeTime >= m_AnimTime) {
			ChangeState(STATE_IDLE, ANIM_IDLE);
		}
	}
}
