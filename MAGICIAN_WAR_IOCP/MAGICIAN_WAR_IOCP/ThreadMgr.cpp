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
	// �ھ� 6��, ������ main : 1��, Work : 6��, Time : 1�� �� 8�� 
	cout << "Create Threads \n";

	Thread_Interface* MyThread = new TimeThread(); // Time 1��
	Thread_Add(MyThread); 

	for (int i = 0; i < WORKTHREAD_COUNT; ++i) { // work 6��
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
