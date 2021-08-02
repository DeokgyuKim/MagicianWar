#pragma once
#include "Component.h"
class Com_Network :
	public Component
{
public:
	Com_Network(int tag, int netEvent);
	~Com_Network();
public:
	virtual void LateUpdate(const float& fTimeDelta);

public:
	// Set
	// Get
public:
	void CallEvent(int _event = 0, int val1 = 0, int val2 = 0, int val3 = 0);
protected:
	int m_NetEvent;
	int m_iTag;

};

