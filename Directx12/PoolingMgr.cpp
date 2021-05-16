#include "PoolingMgr.h"
#include "Bullet.h"

#include "Transform.h"
#include "InstanceMgr.h"

#include "Renderer.h"

PoolingMgr* PoolingMgr::m_pInstance = NULL;

void PoolingMgr::InitPoolingObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* renderer)
{
	//미리 생성할 풀링 오브젝트들 생성
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

void PoolingMgr::UpdatePoolingObject(vector<Client_Bullet> m_vBullets)
{
	//매번 총알 인스턴스 개수를 0으로 초기화
	//cout << m_vBullets.size() << endl;
	InstanceMgr::GetInstnace()->SetInstaneCount("FireBall", 0);

	if (m_vBullets.size() == 0)
		return;

	for (int i = 0; i < m_vBullets.size(); ++i)
	{
		//인스턴스 ObjectCB 업데이트
		if (m_vBullets[i].InstanceName == WIZARD_FIRE)
		{
			InstanceMgr::GetInstnace()->UpdateInstanceBuffer(m_mapInstCnt["FireBall"], "FireBall", XMLoadFloat4x4(&m_vBullets[i].matWorld));
			++m_mapInstCnt["FireBall"];
		}
	}
	

	for (auto elem : m_mapInstCnt)
	{
		InstanceMgr::GetInstnace()->SetInstaneCount(elem.first, elem.second);
		Renderer::GetInstance()->PushObject(RENDER_BULLET, m_mapPoolObject[elem.first]);
	}
	m_mapInstCnt.clear();

	m_vBullets.resize(0);
}

void PoolingMgr::Release()
{
	for (auto elem : m_mapPoolObject)
	{
		delete elem.second;
	}
	m_mapPoolObject.clear();
}
