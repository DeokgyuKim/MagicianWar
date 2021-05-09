#pragma once

#include "include.h"

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
	PLAYER_STATE GetState() const { return static_cast<PLAYER_STATE>(m_State); }

	virtual void Execute() = 0;
protected:
	int m_State;
};
