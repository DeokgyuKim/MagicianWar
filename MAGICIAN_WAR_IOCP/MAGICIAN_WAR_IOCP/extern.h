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
extern mutex g_Client_mutex;
extern map<int, int> g_ConnectedClients_Number;
extern map<int, Room*> g_Rooms;
extern mutex g_Room_mutex;
extern HANDLE g_IOCP;
