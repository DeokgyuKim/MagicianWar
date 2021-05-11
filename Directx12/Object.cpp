#include "Object.h"
#include "InstanceMgr.h"
#include "InstanceInfo.h"
#include "Component.h"

Object::Object(string _meshName)
    :m_strInstName(_meshName)
{
    if (InstanceMgr::GetInstnace()->m_InstanceObjects.find(m_strInstName) != InstanceMgr::GetInstnace()->m_InstanceObjects.end())
    { // 이미 있어
        m_Index = InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName]->GetInstanceCount();
        InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName]->AddInstance(m_strInstName);
    }
    else
    { // 첫생성이야
        m_Index = 0;
        InstanceInfo* _inst = new InstanceInfo(m_strInstName);
        _inst->SetMeshName(m_strInstName);
        InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName] = _inst;
    }
}

Object::Object(string _meshName, MESH_TYPE _meshType)
    :m_strInstName(_meshName), m_strMeshType(_meshType)
{

    if (InstanceMgr::GetInstnace()->m_InstanceObjects.find(m_strInstName) != InstanceMgr::GetInstnace()->m_InstanceObjects.end())
    { // 이미 있어
        m_Index = InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName]->GetInstanceCount();
        InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName]->AddInstance(m_strInstName);
        if (_meshType == MESH_TYPE::CHARACTER)
        {
            InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName]->SetMeshType(_meshType);
        }
    }
    else
    { // 첫생성이야
        m_Index = 0;
        InstanceInfo* _inst = new InstanceInfo(m_strInstName);
        
        if (_meshType == MESH_TYPE::CHARACTER)
        {
            _inst->SetMeshType(_meshType);
        }
        
        _inst->SetMeshName(m_strInstName);
        
        InstanceMgr::GetInstnace()->m_InstanceObjects[m_strInstName] = _inst;
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
    if (m_bPooling && !m_bPoolRender)
        return;
    for (auto iter = m_mapComponent.begin(); iter != m_mapComponent.end(); ++iter)
        (*iter).second->Render(fTimeDelta, _instanceCount);
}

Component* Object::GetTransController()
{
    auto iter = m_mapComponent.find("Transform");
    if (iter == m_mapComponent.end())
        return nullptr;

    return (*iter).second;
}

Component* Object::GetComponent(string strCom)
{
    auto iter = m_mapComponent.find(strCom);
    if (iter == m_mapComponent.end())
        return nullptr;

    return (*iter).second;
}
