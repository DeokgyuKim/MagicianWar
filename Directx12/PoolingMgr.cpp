#include "PoolingMgr.h"
#include "Bullet.h"

PoolingMgr* PoolingMgr::m_pInstance = NULL;

void PoolingMgr::InitPoolingObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* renderer)
{
	//미리 생성할 풀링 오브젝트들 생성
	//FireBall

	for (int i = 0; i < 100; ++i)
	{
		Object* pObj = new Bullet(device, cmdLst, renderer, "FireBall", XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), 0.f);
		m_mapPoolObject["FireBall"].push_back(pObj);
	}
}

Object* PoolingMgr::CreatePoolObject(string instName)
{
	auto iter = m_mapPoolObject.find(instName);
	if(iter == m_mapPoolObject.end())
		return nullptr;

	Object* pObj = (*iter).second.front();
	(*iter).second.pop_front();

	return pObj;
}

void PoolingMgr::DeletePoolObject(Object* obj)
{
	if (obj->GetInstName() == "")
		return;

	if (m_mapPoolObject.find(obj->GetInstName()) == m_mapPoolObject.end())
		return;

	m_mapPoolObject[obj->GetInstName()].push_back(obj);
}

void PoolingMgr::Release()
{
	for (auto elem : m_mapPoolObject)
	{
		for (auto iter = elem.second.begin(); iter != elem.second.end(); ++iter)
		{
			delete (*iter);
			(*iter) = NULL;
		}
		elem.second.clear();
	}
	m_mapPoolObject.clear();
}
