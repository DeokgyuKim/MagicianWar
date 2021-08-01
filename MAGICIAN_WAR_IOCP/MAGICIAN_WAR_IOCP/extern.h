#pragma once


#include "framework.h"
#include "Global.h"
#include "Room.h"

//extern vector<PxRigidStatic*> g_StaticObjects;
extern priority_queue<SOCKET> g_Accept_queue;
extern mutex g_Accept_mutex;

extern priority_queue<EVENT> g_Timer_queue;
extern mutex g_Time_mutex;

extern array<CLIENT_INFO*, MAX_CLIENTS> g_Clients;
extern map<int, Room*> g_Rooms;
extern HANDLE g_IOCP;