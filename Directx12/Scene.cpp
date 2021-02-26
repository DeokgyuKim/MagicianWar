#include "Scene.h"
#include "Object.h"

Scene::~Scene()
{
	Release();
}

int Scene::Update(const float& fTimeDelta)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto Object : m_pObjects[i])
		{
			Object->Update(fTimeDelta);
		}
	}
	return 0;
}

void Scene::LateUpdate(const float& fTimeDelta)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto Object : m_pObjects[i])
		{
			Object->LateUpdate(fTimeDelta);
		}
	}
}

void Scene::Release()
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto Object : m_pObjects[i])
		{
			if(Object != NULL)
				delete Object;
			Object = NULL;
		}
		m_pObjects[i].clear();
	}
}
