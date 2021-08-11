#include "Skill.h"

Skill::Skill()
{
	m_Info.user = NO_PLAYER;
	m_Info.xmfPosition = XMFLOAT3(0.f, 0.f, 0.f);
	m_Info.xmfScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_Info.xmfRotate = XMFLOAT3(0.f, 0.f, 0.f);
	m_Info.LifeTime = 0.f;
	XMStoreFloat4x4(&m_Info.xmmWorld, XMMatrixIdentity());

}

Skill::~Skill()
{
}

int Skill::Update(const float& fTime)
{

	return 0;
}

void Skill::LateUpdate(const float& fTime)
{

}

void Skill::setPosition(XMFLOAT3 pos)
{
	m_Info.xmfPosition = pos;

	m_Info.xmmWorld._41 = pos.x;
	m_Info.xmmWorld._42 = pos.y;
	m_Info.xmmWorld._43 = pos.z;
}

void Skill::setRotate(XMFLOAT3 rotate)
{
	m_Info.xmfRotate = rotate;
}

void Skill::setScale(XMFLOAT3 scale)
{
	m_Info.xmfScale = scale;
}

void Skill::setWorld(XMFLOAT4X4 world)
{
	m_Info.xmmWorld = world;
}
