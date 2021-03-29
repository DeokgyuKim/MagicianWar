#pragma once

#include "framework.h"

class InterfaceAnimation;
class InterfaceCommand;

class AnimationFSM
{
private:
	AnimationFSM();
	AnimationFSM(const AnimationFSM& rhs) = delete;
	~AnimationFSM();
private:
	void Release();
public:
	static AnimationFSM* GetInstacne()
	{
		if (!m_pInstance)
			m_pInstance = new AnimationFSM();
		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static AnimationFSM* m_pInstance;
public:
	// Get
	bool CheckEmpty() const						{ return m_FSM.empty(); }
	
	// Set

public:
	void PushAnimation(int _event, InterfaceAnimation* _IAni);
	void PopAnimation(int _event);
	void Execute();
private:
	list<InterfaceCommand*> m_FSM;

private:

};

