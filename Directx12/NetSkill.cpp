#include "NetSkill.h"
#include "Object.h"
#include "Camera.h"
#include "Transform.h"

XMFLOAT3 GetPositionBulletType(Object* pOwner, Camera* Camera)
{
	XMFLOAT3 pos = dynamic_cast<Transform*>(pOwner->GetTransController())->GetPosition();
	XMFLOAT3 offsetY = XMFLOAT3(0.f, 1.f, 0.f);
	XMFLOAT3 offsetLook = Camera->GetLookVector();

	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&offsetY) + XMLoadFloat3(&offsetLook));
	return pos;
}
XMFLOAT3 GetRotateBulletType(Object* pOwner, Camera* Camera)
{
	XMFLOAT3 Rotate;
	Rotate.x = Camera->GetRotX();
	Rotate.y = Camera->GetRotY();
	Rotate.z = 0.f;
	return Rotate;
}
XMFLOAT3 GetPositionRayCastType(Object* pOwner, Camera* Camera)
{
	bool btemp1, btemp2 = false;
	XMFLOAT3 Pos = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 CamPos = Camera->GetPosition();
	XMFLOAT3 CamDir = Camera->GetLookVector();
	if (CPhysXMgr::GetInstance()->ShotRay(XMLoadFloat3(&CamPos), XMLoadFloat3(&CamDir), 100.f, btemp1, btemp2, Pos))
	{
		return Pos;
	}
	return XMFLOAT3(-99.f, -99.f, -99.f);
}
XMFLOAT3 GetRotateRayCastType(Object* pOwner, Camera* Camera)
{
	XMFLOAT3 Rotate;
	Rotate.x = Camera->GetRotX();
	Rotate.y = Camera->GetRotY();
	Rotate.z = 0.f;
	return Rotate;
}

NetSkill::NetSkill(Object* pOwner, Camera* Camera, SKILL_GENTYPE eSkillType)
{
	m_eSkillType = eSkillType;
	m_pOwner = pOwner;
	m_pCamera = Camera;
}

NetSkill::~NetSkill()
{
}

XMFLOAT3 NetSkill::GetPosition()
{
	switch (m_eSkillType)
	{
	case SKILL_BULLET:
		return GetPositionBulletType(m_pOwner, m_pCamera);
		break;
	case SKILL_RAYCAST:
		return GetPositionRayCastType(m_pOwner, m_pCamera);
		break;
	default:
		break;
	}
	return XMFLOAT3(-99.f, -99.f, -99.f);
}

XMFLOAT3 NetSkill::GetRotate()
{
	switch (m_eSkillType)
	{
	case SKILL_BULLET:
		return GetRotateBulletType(m_pOwner, m_pCamera);
		break;
	case SKILL_RAYCAST:
		return GetRotateRayCastType(m_pOwner, m_pCamera);
		break;
	default:
		break;
	}
	return XMFLOAT3(-99.f, -99.f, -99.f);
}

