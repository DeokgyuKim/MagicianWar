#pragma once
#include "RadioButtonController.h"

class Scene;
class RoomRadioController :
    public RadioButtonController
{
public:
	RoomRadioController(XMFLOAT4 xmfBox);
	~RoomRadioController();
public:
	void	SetScrollPos(int Zdir, float speed);
	int		GetSelectRoomNumber();
public:
	void AddRoom(Scene* pScene, int iTag, bool bAddIdx = true);
	void RemoveRoom(Scene* pScene, int iTag);
public:
	list<int> m_lstRooms;
	float m_fScrollPos = 0.f;
	RECT m_Rect;
};

