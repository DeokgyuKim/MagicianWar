#include "IceFreeze.h"

IceFreeze::IceFreeze()
	:Skill()
{
	m_Info.TotalLifeTime = 10.f;
}

IceFreeze::~IceFreeze()
{
}

bool IceFreeze::Update(const float& fTime)
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

void IceFreeze::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}
