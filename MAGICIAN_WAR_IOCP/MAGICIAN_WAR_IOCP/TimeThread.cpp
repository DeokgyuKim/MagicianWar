#include "TimeThread.h"
#include "extern.h"
#include "Server.h"
#include "protocol.h"
void TimeThread::Initialize()
{
	m_Thread = thread([&]()
	{
		TimeThread::Thread_Run();
	});
}

void TimeThread::Thread_Run()
{
	// Running Thread
	while (true)
	{
		g_Time_mutex.lock();
		while (g_Timer_queue.empty()) // 처리할 이벤트가 없으면 재워놔
		{
			g_Time_mutex.unlock();
			this_thread::sleep_for(10ms);
			g_Time_mutex.lock();
		}

		const EVENT& Ev = g_Timer_queue.top();

		if (chrono::system_clock::now() < Ev.wakeup_time)
		{
			g_Time_mutex.unlock();
			this_thread::sleep_for(1ms);
			continue;
		}

		EVENT Event_proc = Ev;
		g_Timer_queue.pop();
		g_Time_mutex.unlock();

		OVER_EX* over_ex = new OVER_EX;

		if (g_Rooms[Event_proc.Target_ID] != nullptr) { // 방 생성 여부
			// 이벤트 처리
			if (Event_proc.opType == OP_ROOM_UPDATE)
			{
				over_ex->OpType = OP_ROOM_UPDATE;
				*reinterpret_cast<int*>(over_ex->Iocp_buf) = Event_proc.Target_ID;
			}
			else if (Event_proc.opType == OP_ROOM_SEND_PACKET)
			{
				over_ex->OpType = OP_ROOM_SEND_PACKET;
				*reinterpret_cast<int*>(over_ex->Iocp_buf) = Event_proc.Target_ID;
			}
			else if (Event_proc.opType == OP_ROOM_TIME)
			{
				over_ex->OpType = OP_ROOM_TIME;
				*reinterpret_cast<int*>(over_ex->Iocp_buf) = Event_proc.Target_ID;
			}
			//else if (Event_proc.opType = OP_ROOM_BREAK)
			//{
			//	over_ex->OpType = OP_ROOM_BREAK;
			//	*reinterpret_cast<int*>(over_ex->Iocp_buf) = Event_proc.Target_ID;
			//}
			else
			{
				cout << " TimeThread - Thread_Run() - Error " << endl;
			}

		PostQueuedCompletionStatus(g_IOCP, 1, Event_proc.Object_ID, &over_ex->Over);
		}
	}
}



void TimeThread::Join()
{
	m_Thread.join();
}
