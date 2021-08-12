#include "FireWall.h"

FireWall::FireWall()
	:Skill()
{
	m_Info.TotalLifeTime = 5.f;
}

FireWall::~FireWall()
{
}

int FireWall::Update(const float& fTime)
{
	Skill::Update(fTime);

	m_Info.LifeTime += fTime;

	if (m_Info.TotalLifeTime <= m_Info.LifeTime) {

		return 1;
	}

	memcpy(&m_Info.xmfPosition, &m_Info.xmmWorld._41, sizeof(XMFLOAT3));

	return 0;

}

void FireWall::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}
