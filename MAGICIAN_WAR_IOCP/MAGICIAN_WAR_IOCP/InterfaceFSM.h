#pragma once

#include "framework.h"

class InterfaceFSM
{
public:
	explicit InterfaceFSM() {};
	virtual ~InterfaceFSM() {};

public:
	// Set
	virtual void ChangeState(int _State, int _Ani) = 0;
	virtual void Enter(int _State, int _Ani) = 0;
	virtual void Exit() = 0;
	// Get
	int GetState() const { return m_State; }

	virtual void Execute(float fTime) = 0;
protected:
	int m_State;
};
