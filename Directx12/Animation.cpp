#include "Animation.h"
#include "AnimationMgr.h"

AnimationCom::AnimationCom(const string& user)
{
	// �޽��� �´� �ִϸ��̼� �������ְ�
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
	curAnimation = ANIMATION_TYPE::DANCE; // �ϴ� ����
	
	// �ð� �帧
	TimePos += fTimeDelta;

	// ���� �ִϸ��̼�
	m_SkinnedModelInst->UpdateAnimation(curAnimation, TimePos);

	if (TimePos > m_SkinnedModelInst->SkinnedInfo->GetClipEndTime(curAnimation)) {
		TimePos = 0.f;
	}
	return 0;
}
