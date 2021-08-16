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
	
	XMFLOAT3 look;
	memcpy(&look, &m_Info.xmmWorld._31, sizeof(XMFLOAT3));

	XMStoreFloat3(&m_Info.xmfPosition, XMLoadFloat3(&m_Info.xmfPosition) + XMLoadFloat3(&look) * fTime * 10.f);

	//memcpy(&m_Info.xmfPosition, &m_Info.xmmWorld._41, sizeof(XMFLOAT3));

	return m_Info.Dead;
}

void DistortionPearl::LateUpdate(const float& fTime)
{
	Skill::LateUpdate(fTime);
}
