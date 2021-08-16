#include "Animation.h"
#include "AnimationMgr.h"
#include "Network.h"
AnimationCom::AnimationCom(const string& user)
{
	// 메쉬에 맞는 애니메이션들 세팅해주고
	SkinnedModelInstance* inst = new SkinnedModelInstance(*AnimationMgr::GetInstance()->GetAnimations(user));
	SkinnedData* instData = inst->SkinnedInfo.get();
	m_SkinnedModelInst = new SkinnedModelInstance();
	m_SkinnedModelInst->FinalTransforms = inst->FinalTransforms;
	m_SkinnedModelInst->SkinnedInfo = make_unique<SkinnedData>();

	m_SkinnedModelInst->SkinnedInfo->mAnimations = instData->mAnimations;
	m_SkinnedModelInst->SkinnedInfo->mBoneHierarchy = instData->mBoneHierarchy;
	m_SkinnedModelInst->SkinnedInfo->mBoneName = instData->mBoneName;
	m_SkinnedModelInst->SkinnedInfo->mBoneOffsets = instData->mBoneOffsets;
	m_SkinnedModelInst->SkinnedInfo->mSubmeshOffset = instData->mSubmeshOffset;
	m_SkinnedModelInst->SkinnedInfo->m_ToRootTransforms = instData->m_ToRootTransforms;

	curAnimation = make_unique<AnimData>(ANIMATION_TYPE::IDLE, 0.f);
	keyAnimation = make_unique<AnimData>(ANIMATION_TYPE::IDLE, 0.f);

	m_bBlending = false;
	m_fBlendTime = 0.f;
	m_fMaxBlendTime = 10.f;

}

AnimationCom::~AnimationCom()
{
	delete m_SkinnedModelInst;
	m_SkinnedModelInst = nullptr;


}

int AnimationCom::Update(const float& fTimeDelta)
{
	//if (!m_bBlending) {
	//	DefaultAnimate(fTimeDelta);
	//}
	//else {
	BlendingAnimate(fTimeDelta);
	//}

	return 0;
}

void AnimationCom::DefaultAnimate(const float& fTimeDelta)
{
	if (curAnimation->eType == ANIMATION_TYPE::ATTACK)
		curAnimation->Time += fTimeDelta * 5.f;
	else if (curAnimation->eType == ANIMATION_TYPE::JUMP)
		curAnimation->Time += fTimeDelta * 3.f;
	else if (curAnimation->eType == ANIMATION_TYPE::HIT)
		curAnimation->Time += fTimeDelta;
	else
		curAnimation->Time += fTimeDelta * 5.f;

	// 현재 애니메이션
	m_SkinnedModelInst->UpdateAnimation(curAnimation->eType, curAnimation->Time);

	if (curAnimation->Time * 1.4 > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(curAnimation->eType)) {
		m_bAttackEnd = true; // 일반 공격시 공격이 끝나면 Idle로 바꾸기 위한 bool 변수
	}

	if (curAnimation->Time > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(curAnimation->eType)) {
		curAnimation->Time = 0.f;
	}
}

void AnimationCom::BlendingAnimate(const float& fTimeDelta)
{
	if (keyAnimation->eType == ANIMATION_TYPE::ATTACK)
	{
		curAnimation->Time += fTimeDelta * 5.f;
		keyAnimation->Time += fTimeDelta * 5.f;
	}
	else if (keyAnimation->eType == ANIMATION_TYPE::JUMP)
	{
		curAnimation->Time += fTimeDelta * 3.f;
		keyAnimation->Time += fTimeDelta * 3.f;
	}
	else if (keyAnimation->eType == ANIMATION_TYPE::HIT)
	{
		curAnimation->Time += fTimeDelta;
		keyAnimation->Time += fTimeDelta;
	}
	else if (keyAnimation->eType == ANIMATION_TYPE::DEAD)
	{
		curAnimation->Time += fTimeDelta;
		keyAnimation->Time += fTimeDelta;
	}
	else if (keyAnimation->eType == ANIMATION_TYPE::IDLE)
	{
		curAnimation->Time += fTimeDelta;
		keyAnimation->Time += fTimeDelta;
	}
	else if (keyAnimation->eType == ANIMATION_TYPE::DANCE)
	{
		curAnimation->Time += fTimeDelta;
		keyAnimation->Time += fTimeDelta;
	}
	else
	{
		curAnimation->Time += fTimeDelta * 5.f;
		keyAnimation->Time += fTimeDelta * 5.f;
	}


	m_fBlendTime += 45.f * fTimeDelta;
	m_fBlendTime = min(m_fBlendTime, m_fMaxBlendTime);
	m_SkinnedModelInst->UpdateBlendAnimation(
		curAnimation->eType, curAnimation->Time,
		keyAnimation->eType, keyAnimation->Time,
		(m_fBlendTime / m_fMaxBlendTime));

	if (keyAnimation->Time * 1.4 > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(keyAnimation->eType)) {
		if (keyAnimation->eType == ANIMATION_TYPE::ATTACK)
			m_bAttackEnd = true; // 일반 공격시 공격이 끝나면 Idle로 바꾸기 위한 bool 변수
		if (keyAnimation->eType == ANIMATION_TYPE::SKILLATTACK)
			m_bSkillAttackEnd = true;

	}
	if (keyAnimation->eType != ANIMATION_TYPE::DEAD)
	{
		if (keyAnimation->Time > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(keyAnimation->eType)) {
			keyAnimation->Time = 0.f;
		}
	}
	else
	{
		//cout << "현재 애니메이션은 DEAD" << endl;
	}
}



void AnimationCom::ChangeAnimation(int _Ani, bool upper)
{
	ANIMATION_TYPE nextAni = static_cast<ANIMATION_TYPE>(_Ani);
	if (keyAnimation->eType == nextAni)
		return;

	curAnimation->eType = keyAnimation->eType;
	if (keyAnimation->eType != nextAni) { // 애니메이션이 바뀌면 Blending
		m_bBlending = true;
		keyAnimation->eType = nextAni;
		keyAnimation->Time = 0.f;
		m_fBlendTime = 1.f;
		m_bAttackEnd = false;
		m_bSkillAttackEnd = false;
	}

	if (upper)
	{
		//if (nextAni == ANIMATION_TYPE::ATTACK)
		//{
		//	Network::GetInstance()->CallEvent(EVENT_CREATE_BULLET_REQUEST, 0);
		//}
	}
}

AnimData* AnimationCom::getCurAnimation()
{
	if (curAnimation == nullptr)
		return nullptr;
	return curAnimation.get();
}

AnimData* AnimationCom::getkeyAnimation()
{
	if (keyAnimation == nullptr)
		return nullptr;
	return keyAnimation.get();
}



bool AnimationCom::StateCheck(ANIMATION_TYPE _eState)
{
	return false;
}
