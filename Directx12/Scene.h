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
protected:
	list<Object*> m_pObjects[OBJ_TYPE::OBJ_END];
};

