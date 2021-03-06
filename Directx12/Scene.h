#pragma once
#include "framework.h"

class Object;
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
	Object* GetUIForTag(int iTag);
	Object* GetPlayer(); 
	SCENE_TYPE GetSceneType() { return m_eSceneType; }
protected:
	list<Object*> m_pObjects[OBJ_TYPE::OBJ_END]; // Object �����͵�
	SCENE_TYPE m_eSceneType;
};

