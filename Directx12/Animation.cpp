#include "Animation.h"
#include "AnimationMgr.h"

AnimationCom::AnimationCom(const string& user)
{
	// 메쉬에 맞는 애니메이션들 세팅해주고
	SkinnedModelInstance* inst = AnimationMgr::GetInstance()->GetAnimations(user);
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
	if (!m_bBlending) {
		DefaultAnimate(fTimeDelta);
	}
	else {
		BlendingAnimate(fTimeDelta);
	}

	return 0;
}

void AnimationCom::DefaultAnimate(const float& fTimeDelta)
{
	if (curAnimation->eType == ANIMATION_TYPE::ATTACK)
		curAnimation->Time += fTimeDelta * 5.f;
	else if (curAnimation->eType == ANIMATION_TYPE::JUMP)
		curAnimation->Time += fTimeDelta * 3.f;
	else
		curAnimation->Time += fTimeDelta * 5.f;

	// 현재 애니메이션
	m_SkinnedModelInst->UpdateAnimation(curAnimation->eType, curAnimation->Time);

	if (curAnimation->Time > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(curAnimation->eType)) {
		curAnimation->Time = 0.f;

	}
}

void AnimationCom::BlendingAnimate(const float& fTimeDelta)
{
	curAnimation->Time += fTimeDelta;
	keyAnimation->Time += fTimeDelta;

	if (m_fBlendTime > m_fMaxBlendTime) {
		curAnimation->eType = keyAnimation->eType;
		curAnimation->Time = 0.f;
		m_bBlending = false;
	}
	else {
		m_fBlendTime += 60.f * fTimeDelta;
		m_SkinnedModelInst->UpdateBlendAnimation(
			curAnimation->eType, curAnimation->Time,
			keyAnimation->eType, keyAnimation->Time,
			(m_fBlendTime / m_fMaxBlendTime));
	}


}



void AnimationCom::ChangeAnimation(ANIMATION_TYPE eType)
{
	if (keyAnimation->eType != eType) { // 애니메이션이 바뀌면 Blending
		m_bBlending = true;
		keyAnimation->eType = eType;
		keyAnimation->Time = 0.f;
		m_fBlendTime = 1.f;
	}
}



bool AnimationCom::StateCheck(ANIMATION_TYPE _eState)
{
	return false;
}
