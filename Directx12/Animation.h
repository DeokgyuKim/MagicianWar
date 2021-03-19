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
	// 해당 메쉬에 대한 모든 애니메이션을 들고 있다.
	SkinnedModelInstance* m_SkinnedModelInst; 
	ANIMATION_TYPE curAnimation; // 현재 애니메이션

	// 애니메이션 Test
	float TimePos = 0.0f;
};

