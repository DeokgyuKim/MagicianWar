#pragma once

#include "Component.h"
#include "SkinnedModelInstance.h"

class AnimData
{
public:
	AnimData() {}
	AnimData(ANIMATION_TYPE	_etype, float _time) { eType = _etype; Time = _time; }
	~AnimData() = default;
	ANIMATION_TYPE eType;
	float Time;
};

class AnimationCom :
	public Component
{
public:
	AnimationCom(const string& user);
	virtual ~AnimationCom();

public:
	virtual int Update(const float& fTimeDelta);
	void DefaultAnimate(const float& fTimeDelta);
	void BlendingAnimate(const float& fTimeDelta);
public:
	SkinnedModelInstance* GetSkinnedModellnst() { return m_SkinnedModelInst; }

public:
	void ChangeAnimation(int _Ani);
	bool GetAttackEnd() { return m_bAttackEnd; }
private: // 애니메이션Com은 애니메이션으로 비교를 하는게 정배지
	bool StateCheck(ANIMATION_TYPE _eState);
private:
	// 해당 메쉬에 대한 모든 애니메이션을 들고 있다.
	SkinnedModelInstance* m_SkinnedModelInst; 
	unique_ptr<AnimData> curAnimation; // 현재 애니메이션
	unique_ptr<AnimData> keyAnimation; // 눌린 키와 비교할 애니메이션?
	// 애니메이션 Test
	float TimePos = 0.0f;

	bool	m_bBlending;
	float	m_fBlendTime;
	float	m_fMaxBlendTime;

	bool m_bAttackEnd;

};

