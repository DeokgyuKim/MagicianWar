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
private: // �ִϸ��̼�Com�� �ִϸ��̼����� �񱳸� �ϴ°� ������
	bool StateCheck(ANIMATION_TYPE _eState);
private:
	// �ش� �޽��� ���� ��� �ִϸ��̼��� ��� �ִ�.
	SkinnedModelInstance* m_SkinnedModelInst; 
	unique_ptr<AnimData> curAnimation; // ���� �ִϸ��̼�
	unique_ptr<AnimData> keyAnimation; // ���� Ű�� ���� �ִϸ��̼�?
	// �ִϸ��̼� Test
	float TimePos = 0.0f;

	bool	m_bBlending;
	float	m_fBlendTime;
	float	m_fMaxBlendTime;

	bool m_bAttackEnd;

};

