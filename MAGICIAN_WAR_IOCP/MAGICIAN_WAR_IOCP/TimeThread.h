#pragma once
#include "ThreadInterface.h"
class TimeThread :
    public Thread_Interface
{
public:
    TimeThread() {}
    virtual ~TimeThread() {};
public:
    // Thread_Interface��(��) ���� ��ӵ�
    virtual void Initialize() override;
    virtual void Thread_Run() override;
    virtual void Join() override;
};

