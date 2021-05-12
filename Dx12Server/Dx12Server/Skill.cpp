#include "Skill.h"

Skill::Skill()
{
	m_matWorld = MathHelper::Identity4x4();
	m_User = -1;
	m_InstanceName = -1;

	m_TotalLifeTime = 0.f;
	m_speed = 2.f;
	m_lifeTime = 0.f;
}

Skill::Skill(const Skill& rhs)
{
	m_User = rhs.getUser();
	m_InstanceName = rhs.getInstanceName();
	m_matWorld = rhs.getWorld();
	m_TotalLifeTime = rhs.getTotalLifeTime();
	m_speed = rhs.getSpeed();
	m_lifeTime = 0.f;
}

Skill::~Skill()
{
}

void Skill::setWorldMatrix(XMFLOAT4X4 _matWorld)
{
	m_mutex.lock();
	m_matWorld = _matWorld;
	m_mutex.unlock();
}
