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
	void SetScrollPos(int Zdir, float speed);
public:
	void AddRoom(Scene* pScene, int iTag);
	void RemoveRoom(Scene* pScene, int iTag);
public:
	float m_fScrollPos = 0.f;
	RECT m_Rect;
};

