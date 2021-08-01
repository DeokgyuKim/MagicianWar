#pragma once

#include "framework.h"
#include "Global.h"

class Thread_Interface
{
public:
	Thread_Interface() {}
	virtual ~Thread_Interface() {}

public:
	virtual void Initialize() = 0;
	virtual void Thread_Run() = 0;
	virtual void Join() = 0;

protected:
	thread m_Thread;

};

