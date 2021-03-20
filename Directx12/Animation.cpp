#include "Animation.h"
#include "AnimationMgr.h"

AnimationCom::AnimationCom(const string& user)
{
	// 메쉬에 맞는 애니메이션 설정해주고
	m_SkinnedModelInst = AnimationMgr::GetInstance()->GetAnimations(user);
	curAnimation = ANIMATION_TYPE::NONE; 
}

AnimationCom::~AnimationCom()
{
	delete m_SkinnedModelInst;
	m_SkinnedModelInst = nullptr;
}

int AnimationCom::Update(const float& fTimeDelta)
{
	curAnimation = ANIMATION_TYPE::DANCE; // 일단 춤춰
	
	// 시간 흐름
	TimePos += fTimeDelta;

	// 현재 애니메이션
	m_SkinnedModelInst->UpdateAnimation(curAnimation, TimePos);

	if (TimePos > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(curAnimation)) {
		TimePos = 0.f;
	}
	return 0;
}
