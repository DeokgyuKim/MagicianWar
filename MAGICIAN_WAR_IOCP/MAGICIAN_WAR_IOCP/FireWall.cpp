#include "FireWall.h"

FireWall::FireWall()
	:Skill()
{
	m_Info.TotalLifeTime = 5.f;
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
