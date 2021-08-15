#include "NetSkill.h"
#include "Object.h"
#include "Camera.h"
#include "Transform.h"
#include "MainApp.h"
#include "Scene.h"

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
XMFLOAT3 GetPositionRayCastType(Object* pOwner, Camera* Cam)
{
	POINT   ptMouse;

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	XMFLOAT3   vMousePos;

	// 뷰포트에서 투영으로 변환
	vMousePos.x = ptMouse.x / (WINCX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(WINCY * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영에서 뷰스페이스로

	XMMATRIX      matProj, matView;

	XMFLOAT4X4 proj, view;

	proj = dynamic_cast<Camera*>(MainApp::GetInstance()->GetScene()->GetCamera())->GetProjMatrix();
	matProj = XMLoadFloat4x4(&proj);
	matProj = XMMatrixInverse(nullptr, matProj);
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), matProj));

	XMFLOAT3   vRayPos, vRayDir;

	vRayPos = XMFLOAT3(0.f, 0.f, 0.f);
	XMStoreFloat3(&vRayDir, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&vRayPos));


	// 뷰 스페이스에서 월드로
	view = dynamic_cast<Camera*>(MainApp::GetInstance()->GetScene()->GetCamera())->GetViewMatrix();
	matView = XMLoadFloat4x4(&view);
	matView = XMMatrixInverse(nullptr, matView);

	XMStoreFloat3(&vRayPos, XMVector3TransformCoord(XMLoadFloat3(&vRayPos), matView));
	XMStoreFloat3(&vRayDir, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&vRayDir), matView)));
	//D3DXVECTOR3      vCameraPos(0.f, 0.f, 0.f);
	//
	//// 월드에서 로컬로 
	//_matrix      matWorld;
	//m_pTransformCom->Get_WorldMatrix(&matWorld);
	//D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	//
	//D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	//D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);




	XMFLOAT3 point = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 normal = XMFLOAT3(0.f, 1.f, 0.f);
	XMVECTOR plane = XMPlaneFromPointNormal(XMLoadFloat3(&point), XMLoadFloat3(&normal));
	
	
	XMVECTOR Collpos = XMPlaneIntersectLine(plane, XMLoadFloat3(&vRayPos), XMLoadFloat3(&vRayPos) + XMLoadFloat3(&vRayDir));
	XMFLOAT3 Coll;
	XMStoreFloat3(&Coll, Collpos);




	bool btemp1, btemp2 = false;
	XMFLOAT3 Pos = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 CamPos = Cam->GetPosition();
	XMFLOAT3 CamDir = Cam->GetLookVector();
	if (CPhysXMgr::GetInstance()->ShotRay(XMLoadFloat3(&CamPos), XMLoadFloat3(&CamDir), 100.f, btemp1, btemp2, Pos))
	{
		XMFLOAT3 fDist1, fDist2;
		XMStoreFloat3(&fDist1, XMVector3Length(XMLoadFloat3(&vRayPos) - XMLoadFloat3(&Pos)));
		XMStoreFloat3(&fDist2, XMVector3Length(XMLoadFloat3(&vRayPos) - XMLoadFloat3(&Coll)));
		
		if(fDist1.x < fDist2.x || vRayDir.y >= 0.f)
			return XMFLOAT3(-99.f, -99.f, -99.f);
	}
	if (vRayDir.y >= 0.f)
		return XMFLOAT3(-99.f, -99.f, -99.f);
	return Coll;
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
	case SKILL_METEOR:
		XMFLOAT3 pos = GetPositionRayCastType(m_pOwner, m_pCamera);
		pos.y += 15.f;
		return pos;
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
	case SKILL_METEOR:
		return XMFLOAT3(-90.f, 0.f, 0.f);
	default:
		break;
	}
	return XMFLOAT3(-99.f, -99.f, -99.f);
}

