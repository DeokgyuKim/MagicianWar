#include "IcaAgeBall.h"

IcaAgeBall::IcaAgeBall()
    :Skill()
{
    m_Info.TotalLifeTime = 10.f;
    m_Info.fSpeed = 1.f;
}

IcaAgeBall::~IcaAgeBall()
{
    
}

bool IcaAgeBall::Update(const float& fTime)
{
    Skill::Update(fTime);
    //cout << m_Info.LifeTime << endl;

    m_Info.LifeTime += fTime;

    if (m_Info.TotalLifeTime <= m_Info.LifeTime) {
        m_Info.Dead = true;
        return m_Info.Dead;
    }

    memcpy(&m_Info.xmfPosition, &m_Info.xmmWorld._41, sizeof(XMFLOAT3));
    
    //XMFLOAT3 dir = XMFLOAT3(m_Info.xmmWorld._31, m_Info.xmmWorld._32, m_Info.xmmWorld._33);
    ////dir.x = -m_Info.xmfRotate.x;

    //XMStoreFloat3(&dir, XMVector3Normalize(XMLoadFloat3(&dir)));
    //XMStoreFloat3(&m_Info.xmfPosition, XMLoadFloat3(&m_Info.xmfPosition) + XMLoadFloat3(&dir) * m_Info.fSpeed * fTime);
    return m_Info.Dead;
}

void IcaAgeBall::LateUpdate(const float& fTime)
{
    Skill::LateUpdate(fTime);
    for (auto rigidbody : m_vecRigidDynamic)
        rigidbody->setGlobalPose(CPhysXMgr::GetInstance()->MakePxTransform(m_Info.xmmWorld));
}

void IcaAgeBall::MakeCollision()
{
    m_vecRigidDynamic.push_back(CPhysXMgr::GetInstance()->CreateSphere(m_Info.xmfPosition, 2.f));
}
