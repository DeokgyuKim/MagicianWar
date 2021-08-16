#include "FireWall.h"

FireWall::FireWall()
	:Skill()
{
	m_Info.TotalLifeTime = 10.f;
}

FireWall::~FireWall()
{
}

bool FireWall::Update(const float& fTime)
{
	Skill::Update(fTime);

	m_Info.LifeTime += fTime;

	if (m_Info.TotalLifeTime <= m_Info.LifeTime) {
		m_Info.Dead = true;
		return m_Info.Dead;
	}

	memcpy(&m_Info.xmfPosition, &m_Info.xmmWorld._41, sizeof(XMFLOAT3));

	return m_Info.Dead;

}

void FireWall::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}

void FireWall::MakeCollision()
{
	LateUpdate(0.f);
	XMFLOAT3 pos, offset;
	pos = m_Info.xmfPosition;
	memcpy(&offset, &m_Info.xmmWorld._11, sizeof(XMFLOAT3));

	for (int i = -2; i <= 2; ++i)
	{
		XMFLOAT3 position;
		XMStoreFloat3(&position, XMLoadFloat3(&pos) + XMLoadFloat3(&offset) * i * 2.f);
		m_vecRigidDynamic.push_back(CPhysXMgr::GetInstance()->CreateSphere(position, 1.f));
	}
}
