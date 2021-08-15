#include "DistortionPearl.h"

DistortionPearl::DistortionPearl()
    :Skill()
{
    m_Info.TotalLifeTime = 10.f;
}

DistortionPearl::~DistortionPearl()
{
}

bool DistortionPearl::Update(const float& fTime)
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

void DistortionPearl::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}
