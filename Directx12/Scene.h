#pragma once
#include "framework.h"

class Object;
class RoomRadioController;
class Scene
{
public:
	Scene() {}
	virtual ~Scene();
public:
	virtual int		Update(const float& fTimeDelta);
	virtual void	LateUpdate(const float& fTimeDelta);
	virtual void	Release();
	virtual void	BuildMaterialCBs();
	virtual void	BuildInstanceCBs();

public:
	void	PushObject(Object* pobj, OBJ_TYPE eType) { m_pObjects[eType].push_back(pobj); }
	void	RemoveObject(Object* pobj, OBJ_TYPE eType);
	Object* GetUIForTag(int iTag);
	Object* GetPlayer(); 
	RoomRadioController* GetRadioButtonConteroller() { return m_rbController; }
	SCENE_TYPE GetSceneType() { return m_eSceneType; }
protected:
	list<Object*> m_pObjects[OBJ_TYPE::OBJ_END]; // Object 데이터들
	SCENE_TYPE m_eSceneType;
	RoomRadioController* m_rbController;
};

