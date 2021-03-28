#include "AnimationController.h"
#include "KeyMgr.h"
#include "InterfaceAnimation.h"
#include "Animation.h"
#include "AnimationFSM.h"

ANIMATION_TYPE& operator++(ANIMATION_TYPE& e) {
	if (e == ANIMATION_TYPE::NONE) {
		throw out_of_range("enum for loop 실패");
	}
	e = ANIMATION_TYPE(static_cast<underlying_type<ANIMATION_TYPE>::type>(e) + 1);
	return e;
}

AnimationController::AnimationController()
	: m_bAttack(false), m_bAttackCool(false), m_bJump(false),
	m_fAttackTime(95.f), m_fAttackDeltaTime(0.f),
	m_fJumpTime(45.f), m_fJumpDeltaTime(0.f)
{
	//InterfaceAnimation* pCommand = dynamic_cast<AnimationCom*>(_pAni);
	
}

AnimationController::~AnimationController()
{
	Release();
}

void AnimationController::Initialize(InterfaceAnimation* _pAni)
{
	for (ANIMATION_TYPE i = ANIMATION_TYPE::IDLE; i < ANIMATION_TYPE::NONE; ++i) {
		m_AnimData[i] = new AnimData(i, 0.f);
	}
	m_pAnimation = _pAni;

	m_automata.push(m_AnimData[ANIMATION_TYPE::IDLE]); // 초기 상태는 IDLE 이니까
	curAnimation = m_automata.top()->eType;


	m_AnimationFSM = AnimationFSM::GetInstacne();
	m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::IDLE), m_pAnimation);

}

void AnimationController::MouseCallback()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (m_bJump) return;

		m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::ATTACK), m_pAnimation);
		m_bAttack = true;

		if (m_bAttackCool)
		{
			m_bAttackCool = false; // 어택의 끝일때 파이어볼 ㄱㄱ
		}
	}
}

void AnimationController::Handler(const float& fTimeDelta)
{
	MouseCallback();
	KeyDown();
	KeyUp();


	CheckAnimation(fTimeDelta);

}

void AnimationController::KeyDown()
{
	if (m_bJump) return; // 점프중에는 애니메이션 안바꿀거야

	if (KeyMgr::GetInstance()->KeyDown('W'))
	{ // 앞
		m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::WALK_FOWARD), m_pAnimation);
	}
	if (KeyMgr::GetInstance()->KeyDown('S'))
	{ // 뒤
		m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::WALK_BACK), m_pAnimation);
	}
	if (KeyMgr::GetInstance()->KeyDown('A'))
	{ // 왼
		m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::WALK_LEFT), m_pAnimation);
	}
	if (KeyMgr::GetInstance()->KeyDown('D'))
	{ // 오
		m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::WALK_RIGHT), m_pAnimation);
	}
	if (KeyMgr::GetInstance()->KeyDown(VK_SPACE))
	{ // 점프
		m_AnimationFSM->PushAnimation(static_cast<int>(ANIMATION_TYPE::JUMP), m_pAnimation);
		m_bJump = true;
	}
}

void AnimationController::KeyUp()
{
	if (KeyMgr::GetInstance()->KeyUp('W'))
	{
		m_AnimationFSM->PopAnimation(static_cast<int>(ANIMATION_TYPE::WALK_FOWARD));
	}
	if (KeyMgr::GetInstance()->KeyUp('S'))
	{
		m_AnimationFSM->PopAnimation(static_cast<int>(ANIMATION_TYPE::WALK_BACK));
	}
	if (KeyMgr::GetInstance()->KeyUp('A'))
	{
		m_AnimationFSM->PopAnimation(static_cast<int>(ANIMATION_TYPE::WALK_LEFT));
	}
	if (KeyMgr::GetInstance()->KeyUp('D'))
	{
		m_AnimationFSM->PopAnimation(static_cast<int>(ANIMATION_TYPE::WALK_RIGHT));
	}


}

void AnimationController::Release()
{

}

void AnimationController::ChangeAnimation()
{
	curAnimation = m_automata.top()->eType;
	m_pAnimation->ChangeAnimation(curAnimation);
}

void AnimationController::LoopTime(const float fTimeDelta)
{
	m_automata.top()->Time += fTimeDelta;
	TimePos = m_automata.top()->Time;

	if (TimePos > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(curAnimation)) {
		m_automata.top()->Time = 0.f;
	}
	//m_pAnimation->LoopTime(TimePos);
}

void AnimationController::CheckAnimation(const float fTimeDelta)
{
	if (m_AnimationFSM->CheckEmpty()) return;
	if (m_bAttack)
	{
		m_fAttackDeltaTime += 60.f * fTimeDelta;
		if (m_fAttackTime < m_fAttackDeltaTime)
		{
			m_bAttack = false;
			m_bAttackCool = true;
			m_fAttackDeltaTime = 0.f;
			m_AnimationFSM->PopAnimation(static_cast<int>(ANIMATION_TYPE::ATTACK));
		}
	}
	else if (m_bJump)
	{
		m_fJumpDeltaTime += 60.f * fTimeDelta;
		if (m_fJumpTime < m_fJumpDeltaTime)
		{
			m_bJump = false;
			m_fJumpDeltaTime = 0.f;
			m_AnimationFSM->PopAnimation(static_cast<int>(ANIMATION_TYPE::JUMP));
		}
	}
	m_AnimationFSM->Execute();
}


void AnimationController::Set_SkinnedModelInst(SkinnedModelInstance* _inst)
{
	m_SkinnedModelInst = _inst;
}

void AnimationController::SetAnimaionKey(DWORD _key)
{
}
