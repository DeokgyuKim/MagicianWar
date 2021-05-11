#include "Bullet.h"

Bullet::Bullet()
{
	m_matWorld = MathHelper::Identity4x4();
	m_User = -1;
	m_InstanceName = -1;
	
	m_TotalLifeTime = 0.f;
	m_speed = 2.f;

	Initialize();
}

Bullet::Bullet(const Bullet& rhs)
{
	m_User = rhs.getUser();
	m_InstanceName = rhs.getInstanceName();
	m_matWorld = rhs.getWorld();
	m_TotalLifeTime = rhs.getTotalLifeTime();
	m_speed = rhs.getSpeed();

	Initialize();
}

void Bullet::Initialize()
{
	m_lifeTime = 0.f;
}

int Bullet::Update(const float gTime)
{
	m_lifeTime += gTime;
	if (m_TotalLifeTime <= m_lifeTime)
		return 1; // dead

	
	memcpy(&m_xmfPosition, &m_matWorld._41, sizeof(XMFLOAT3));
	XMFLOAT3 dir = m_dir;
	XMStoreFloat3(&dir, XMVector3Normalize(XMLoadFloat3(&dir)));
	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&dir) * m_speed * gTime);
	
	return 0;
}

void Bullet::LateUpdate(const float gTime)
{
	XMMATRIX scale, rotateX, rotateY, rotateZ, transform;

	scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_xmfScale));

	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z));

	transform = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));

	XMMATRIX world = scale * rotateX * rotateY * rotateZ * transform;
	XMStoreFloat4x4(&m_matWorld, world);
}

void Bullet::setWorldMatrix(XMFLOAT4X4 _matWorld)
{
	m_mutex.lock();
	m_matWorld = _matWorld;
	m_mutex.unlock();
}
