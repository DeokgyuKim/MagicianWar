#include "Object.h"
#include "InstanceMgr.h"
#include "InstanceInfo.h"
#include "Component.h"

Object::Object(string _meshName)
    :m_strMeshName(_meshName)
{
    if (InstanceMgr::GetInstnace()->m_InstanceObjects.find(m_strMeshName) != InstanceMgr::GetInstnace()->m_InstanceObjects.end())
    { // 이미 있어
        m_Index = InstanceMgr::GetInstnace()->m_InstanceObjects[m_strMeshName]->GetInstanceCount();
        InstanceMgr::GetInstnace()->m_InstanceObjects[m_strMeshName]->AddInstance(m_strMeshName);
    }
    else
    { // 첫생성이야
        m_Index = 0;
        InstanceInfo* _inst = new InstanceInfo(m_strMeshName);
        _inst->SetMeshName(m_strMeshName);
        InstanceMgr::GetInstnace()->m_InstanceObjects[m_strMeshName] = _inst;
    }
}

Object::~Object()
{
    for (auto iter = m_mapComponent.begin(); iter != m_mapComponent.end(); ++iter)
        delete (*iter).second;
    m_mapComponent.clear();

}

int Object::Update(const float& fTimeDelta)
{
    for (auto iter = m_mapComponent.begin(); iter != m_mapComponent.end(); ++iter)
        (*iter).second->Update(fTimeDelta);
    return 0;
}

void Object::LateUpdate(const float& fTimeDelta)
{
    for (auto iter = m_mapComponent.begin(); iter != m_mapComponent.end(); ++iter)
        (*iter).second->LateUpdate(fTimeDelta);
}

void Object::Render(const float& fTimeDelta, int _instanceCount)
{
    for (auto iter = m_mapComponent.begin(); iter != m_mapComponent.end(); ++iter)
        (*iter).second->Render(fTimeDelta, _instanceCount);
}
