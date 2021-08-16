#include "IceCressvas.h"

IceCressvas::IceCressvas()
    :Skill()
{
    m_Info.TotalLifeTime = 5.f;
}

IceCressvas::~IceCressvas()
{
}

bool IceCressvas::Update(const float& fTime)
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

void IceCressvas::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}
