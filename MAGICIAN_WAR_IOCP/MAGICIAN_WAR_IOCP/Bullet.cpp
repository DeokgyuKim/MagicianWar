#include "Bullet.h"
#include "Global.h"


Bullet::Bullet()
{
	m_Info.matWorld = MathHelper::Identity4x4();
	m_Info.user = NO_PLAYER;
	m_Info.ElementType = ELEMENT_FIRE;
	m_Info.Damage = 10;
	m_Info.Team = TEAM_NONE;
	m_TotalLifeTime = 0.f;
	m_speed = 10.f;
	m_xmfScale = XMFLOAT3{ 0.3f,0.3f, 0.3f };
	m_xmfRotate = XMFLOAT3{ 0.f,0.f,0.f };
	m_pRigidDynamic = nullptr;
	//Initialize();
}

Bullet::Bullet(const Bullet& rhs)
{
	m_Info.user = rhs.getUser();
	m_Info.Damage = rhs.getDamage();
	m_Info.ElementType = rhs.getElementType();
	m_Info.matWorld = rhs.getWorld();
	m_Info.Team = rhs.getCheckUserTeam();
	m_TotalLifeTime = rhs.getTotalLifeTime();
	m_lifeTime = rhs.getLifeTime();
	m_speed = rhs.m_speed;

	m_xmfScale = rhs.m_xmfScale;
	m_xmfRotate = rhs.m_xmfRotate;
	m_xmfPosition = rhs.m_xmfPosition;

	Initialize();
}

void Bullet::Release()
{
	if (m_pRigidDynamic != NULL)
		m_pRigidDynamic->release();
}

void Bullet::Initialize()
{
	m_lifeTime = 0.f;
	m_pRigidDynamic = CPhysXMgr::GetInstance()->CreateSphere(m_xmfPosition, 0.25f);
}

int Bullet::Update(const float gTime)
{
	m_lifeTime += gTime;
	if (m_TotalLifeTime <= m_lifeTime) {
		Release();
		
		return 1; // dead
	}

	memcpy(&m_xmfPosition, &m_Info.matWorld._41, sizeof(XMFLOAT3));
	XMFLOAT3 dir = m_dir;
	XMStoreFloat3(&dir, XMVector3Normalize(XMLoadFloat3(&dir)));
	XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&dir) * m_speed * gTime);

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
	XMStoreFloat4x4(&m_Info.matWorld, world);


	m_pRigidDynamic->setGlobalPose(CPhysXMgr::GetInstance()->MakePxTransform(m_Info.matWorld));
}

void Bullet::setElementType(unsigned char _element)
{
	m_Info.ElementType = _element;
	if (m_Info.ElementType == ELEMENT_FIRE)
	{
		setDamage(BULLET_DAMAGE);
	}
	else if (m_Info.ElementType == ELEMENT_COLD)
	{
		setDamage(BULLET_DAMAGE);
	}
	else if (m_Info.ElementType == ELEMENT_DARKNESS)
	{
		setDamage(BULLET_DAMAGE);
	}
}

void Bullet::setWorldMatrix(XMFLOAT4X4 _matWorld)
{
	m_Info.matWorld = _matWorld;	
}
