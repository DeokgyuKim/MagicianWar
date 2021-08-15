#include "Meteor.h"

Meteor::Meteor()
	:Skill()
{
	m_Info.TotalLifeTime = 10.f;
}

Meteor::~Meteor()
{
}

bool Meteor::Update(const float& fTime)
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

void Meteor::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}
