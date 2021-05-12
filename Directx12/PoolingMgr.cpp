#include "PoolingMgr.h"
#include "Bullet.h"

#include "Transform.h"
#include "InstanceMgr.h"

#include "Renderer.h"

PoolingMgr* PoolingMgr::m_pInstance = NULL;

void PoolingMgr::InitPoolingObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* renderer)
{
	//�̸� ������ Ǯ�� ������Ʈ�� ����
	//FireBall

	Object* pObj = new Bullet(device, cmdLst, renderer, "FireBall", XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), 0.f);
	m_mapPoolObject["FireBall"] = pObj;
	m_mapInstCnt["FireBall"] = 0;
}

//Object* PoolingMgr::CreatePoolObject(string instName)
//{
//	auto iter = m_mapPoolObject.find(instName);
//	if(iter == m_mapPoolObject.end())
//		return nullptr;
//
//	Object* pObj = (*iter).second.front();
//	(*iter).second.pop_front();
//
//	return pObj;
//}
//
//void PoolingMgr::DeletePoolObject(Object* obj)
//{
//	if (obj->GetInstName() == "")
//		return;
//
//	if (m_mapPoolObject.find(obj->GetInstName()) == m_mapPoolObject.end())
//		return;
//
//	m_mapPoolObject[obj->GetInstName()].push_back(obj);
//}

void PoolingMgr::UpdatePoolingObject(STOC_Bullet* bullets)
{
	//�Ź� �Ѿ� �ν��Ͻ� ������ 0���� �ʱ�ȭ
	InstanceMgr::GetInstnace()->SetInstaneCount("FireBall", 0);

	if (bullets == NULL)
		return;

	for (int i = 0; i < bullets->Bullet_Count; ++i)
	{
		//�ν��Ͻ� ObjectCB ������Ʈ
		if (bullets->bullets[i].InstanceName == WIZARD_FIRE)
		{
			InstanceMgr::GetInstnace()->UpdateInstanceBuffer(m_mapInstCnt["FireBall"], "FireBall", XMLoadFloat4x4(&bullets->bullets[i].matWorld));
			++m_mapInstCnt["FireBall"];
		}
	}
	

	for (auto elem : m_mapInstCnt)
	{
		InstanceMgr::GetInstnace()->SetInstaneCount(elem.first, elem.second);
		Renderer::GetInstance()->PushObject(RENDER_BULLET, m_mapPoolObject[elem.first]);
	}
	m_mapInstCnt.clear();

	bullets = NULL;
}

void PoolingMgr::Release()
{
	for (auto elem : m_mapPoolObject)
	{
		delete elem.second;
	}
	m_mapPoolObject.clear();
}
