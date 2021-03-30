#pragma once

#include "framework.h"

class Player;
class InterfaceFSM
{
public:
	explicit InterfaceFSM(Player* user) : m_User(user) {}
	virtual ~InterfaceFSM() {};

public:
	// Set
	virtual void SetState(int cEvent) = 0;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	// Get
	int GetState() const { return m_State; }
	
	virtual void Execute() = 0;
protected:
	Player* m_User;
	int m_State;
};

