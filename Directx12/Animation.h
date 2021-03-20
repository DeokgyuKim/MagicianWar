#pragma once

#include "Component.h"
#include "SkinnedModelInstance.h"

class AnimationCom :
	public Component
{
public:
	AnimationCom(const string& user);
	virtual ~AnimationCom();

public:
	virtual int Update(const float& fTimeDelta);
public:
	SkinnedModelInstance* GetSkinnedModellnst() { return m_SkinnedModelInst; }
private:
	// �ش� �޽��� ���� ��� �ִϸ��̼��� ��� �ִ�.
	SkinnedModelInstance* m_SkinnedModelInst; 
	ANIMATION_TYPE curAnimation; // ���� �ִϸ��̼�

	// �ִϸ��̼� Test
	float TimePos = 0.0f;
};

