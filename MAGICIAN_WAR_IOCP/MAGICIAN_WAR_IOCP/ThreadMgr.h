#pragma once

#include "framework.h"

class Thread_Interface;
class ThreadMgr
{
private:
	ThreadMgr() {}
	ThreadMgr(const ThreadMgr& rhs) {}
	~ThreadMgr() {}
public:
	static ThreadMgr* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new ThreadMgr();
		return m_pInstance;
	}
	static void DestoryInstance() {
		if (m_pInstance != NULL)
		{
			m_pInstance->Release();
			delete m_pInstance;
		}
		m_pInstance = NULL;
	}
private:
	static ThreadMgr* m_pInstance;
private:
	void	Release();
public:
	void Initialize();

public:
	void Threads_Create();
	void Threads_Join();

	void Thread_Add(Thread_Interface* _thread);

private:
	vector<Thread_Interface*> m_Threads;
};

