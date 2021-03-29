#pragma once

#include "framework.h"

class InterfaceAnimation;
class InterfaceCommand
{
public:
	explicit InterfaceCommand(InterfaceAnimation* user) : m_User(user) {}
	virtual ~InterfaceCommand() {};

public:
	// Set
	virtual void SetState(int cEvent) { m_State = cEvent; }
	// Get
	int GetState() const { return m_State; }
	
	virtual void Execute() = 0;
protected:
	InterfaceAnimation* m_User;
	int m_State;
};

