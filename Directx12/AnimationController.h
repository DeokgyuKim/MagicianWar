#pragma once

#include "framework.h"
#include "SkinnedModelInstance.h"

class InterfaceAnimation;
class AnimationFSM;
class AnimData;

class AnimationController
{
public:
	explicit AnimationController();
	virtual ~AnimationController();

public:
	void Initialize(InterfaceAnimation* _pAni);
	void Release();
	void LoopTime(const float fTimeDelta);
	void CheckAnimation(const float fTimeDelta);
public:
	// Set
	void Set_SkinnedModelInst(SkinnedModelInstance* _inst);
	void SetAnimaionKey(DWORD _key);
	void SetJumpEnd(int _check);
public:
	void MouseCallback();
	void Handler(const float& fTimeDelta);
	void KeyDown();
	void KeyUp();
private:
	ANIMATION_TYPE curAnimation; // ���� �ִϸ��̼�
	float TimePos = 0.0f;
	ANIMATION_TYPE AnimationKey; // ���� Ű�� ���� �ִϸ��̼�?

	InterfaceAnimation* m_pAnimation; // ������ �ִϸ��̼�
	SkinnedModelInstance* m_SkinnedModelInst;

	unordered_map<ANIMATION_TYPE, AnimData*> m_AnimData; // 
	stack<AnimData*> m_automata;

	AnimationFSM*	m_AnimationFSM;

	bool bCheck = false;

	bool m_bAttack;
	bool m_bAttackCool;
	float m_fAttackTime;
	float m_fAttackDeltaTime;

	bool m_bJump;
	float m_fJumpTime;
	float m_fJumpDeltaTime;
};

