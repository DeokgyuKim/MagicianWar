#include "Object.h"
#include "Component.h"

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

void Object::Render(const float& fTimeDelta)
{
    for (auto iter = m_mapComponent.begin(); iter != m_mapComponent.end(); ++iter)
        (*iter).second->Render(fTimeDelta);
}
