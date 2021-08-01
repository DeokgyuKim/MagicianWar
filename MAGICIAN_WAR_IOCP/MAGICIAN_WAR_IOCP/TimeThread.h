#pragma once
#include "ThreadInterface.h"
class TimeThread :
    public Thread_Interface
{
public:
    TimeThread() {}
    virtual ~TimeThread() {};
public:
    // Thread_Interface을(를) 통해 상속됨
    virtual void Initialize() override;
    virtual void Thread_Run() override;
    virtual void Join() override;
};

