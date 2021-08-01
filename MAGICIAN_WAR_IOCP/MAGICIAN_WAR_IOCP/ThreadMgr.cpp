#include "ThreadMgr.h"
#include "Global.h"
#include "ThreadInterface.h"
#include "WorkThread.h"
#include "TimeThread.h"

ThreadMgr* ThreadMgr::m_pInstance = NULL;

void ThreadMgr::Release()
{
}

void ThreadMgr::Initialize()
{
}

void ThreadMgr::Threads_Create()
{
	// 코어 6개, 쓰레드 main : 1개, Work : 6개, Time : 1개 총 8개 
	cout << "Create Threads \n";

	Thread_Interface* MyThread = new TimeThread(); // Time 1개
	Thread_Add(MyThread); 

	for (int i = 0; i < WORKTHREAD_COUNT; ++i) { // work 6개
		MyThread = new WorkThread();
		Thread_Add(MyThread);
	}
	
	cout << "CreateThreads() Finished\n";
}

void ThreadMgr::Thread_Add(Thread_Interface* _thread)
{
	_thread->Initialize();
	m_Threads.emplace_back(_thread);
}

void ThreadMgr::Threads_Join()
{
	for (auto& thread : m_Threads)
		thread->Join();
}
