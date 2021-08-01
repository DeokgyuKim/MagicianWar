#pragma once
#include "ThreadInterface.h"
class WorkThread :
    public Thread_Interface
{
public:
    WorkThread();
    virtual ~WorkThread() {};
public:
    // Thread_Interface을(를) 통해 상속됨
    virtual void Initialize() override;
    virtual void Thread_Run() override;
    virtual void Join() override;

public:
    void Accept_Client(SOCKET client);
    void Disconnect_Client(int client_Num, SOCKET client);
public:
    ROOM_EVENT packetProcessing(int id, void* buffer);
    
private:
    SOCKADDR_IN m_ServerAddress;

};

