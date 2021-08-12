#include "Scene.h"
#include "Object.h"
#include "MaterialMgr.h"
#include "InstanceMgr.h"
#include "InstanceInfo.h"

#include "UI.h"
#include "Player.h"
#include "Skill.h"

Scene::~Scene()
{
	Release();
}

int Scene::Update(const float& fTimeDelta)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto iter = m_pObjects[i].begin(); iter != m_pObjects[i].end();)
		{
			int Event = (*iter)->Update(fTimeDelta);
			if (Event == -1)
			{
				delete* iter;
				iter = m_pObjects[i].erase(iter);
			}
			else
				++iter;
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
			InstanceMgr::GetInstnace()->UpdateInstanceBuffer(Object);
			InstanceMgr::GetInstnace()->UpdateSkinnedBuffers(Object);
		}
	}
	MaterialMgr::GetInstnace()->UpdateMaterialCBs();
	
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

void Scene::BuildMaterialCBs()
{
	MaterialMgr::GetInstnace()->BuildMaterialCBs();
}

void Scene::BuildInstanceCBs()
{
	InstanceMgr::GetInstnace()->BuildInstanceBuffers();
}

void Scene::RemoveObject(Object* pobj, OBJ_TYPE eType)
{
	auto eraseiter = m_pObjects[eType].end();
	for (auto iter = m_pObjects[eType].begin(); iter != m_pObjects[eType].end(); ++iter)
	{
		if (pobj == (*iter))
		{
			eraseiter = iter;
			break;
		}
	}
	if (eraseiter == m_pObjects[eType].end())
		return;
	m_pObjects[eType].erase(eraseiter);
}

Object* Scene::GetUIForTag(int iTag)
{
	for (auto iter = m_pObjects[OBJ_TYPE::OBJ_UI].begin(); iter != m_pObjects[OBJ_TYPE::OBJ_UI].end(); ++iter)
	{
		if (dynamic_cast<UI*>(*iter)->GetTag() == iTag)
			return (*iter);
	}
	return nullptr;
}

Object* Scene::GetPlayerForID(int id)
{
	for (auto iter = m_pObjects[OBJ_PLAYER].begin(); iter != m_pObjects[OBJ_PLAYER].end(); ++iter)
	{
		if (dynamic_cast<Player*>(*iter)->GetNetworkInfo().Client_Num == id)
			return (*iter);
	}
	return nullptr;
}

Object* Scene::GetPlayer()
{
	if(m_pObjects[OBJ_PLAYER].empty())
		return nullptr;
	return m_pObjects[OBJ_PLAYER].front();
}

Object* Scene::GetCamera()
{
	if (m_pObjects[OBJ_CAMERA].empty())
		return nullptr;
	return m_pObjects[OBJ_CAMERA].front();
}

Object* Scene::GetSkillForSlot(SKILL_TYPE eSkill, unsigned char cSlot)
{
	for (auto skill : m_pObjects[OBJ_SKILL])
		if (dynamic_cast<Skill*>(skill)->GetSkillType() == eSkill)
			if (dynamic_cast<Skill*>(skill)->GetSlotNum() == cSlot)
				return skill;
	return nullptr;
}
