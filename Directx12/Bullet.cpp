#include "Bullet.h"

#include "MainApp.h"
#include "Scene.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "Core.h"
#include "Player.h"

 // Component
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Animation.h"

Bullet::Bullet(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string _meshName,
	XMFLOAT3 Pos, XMFLOAT3 Rotate, float speed)
	:Object(_meshName)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	m_fSpeed = speed;

	Initialize(Pos, Rotate);

	
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(XMFLOAT3 Pos, XMFLOAT3 Rotate)
{
	Core::GetInstance()->CmdLstReset();


	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), Rotate, Pos);
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Sphere");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("Sphere");
	m_mapComponent["Material"] = pComponent;

	m_strTextureName = "FireBall3";
	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	//PhysX
#ifdef PHYSX
	m_pRigidDynamic = CPhysXMgr::GetInstance()->CreateSphere(this, Pos, 1.f);
	m_pRigidDynamic->setMass(10.f);
	//m_pRigidDynamic->addForce(CPhysXMgr::GetInstance()->ToPxVec3(XMFLOAT3(0.f, 0.f, 100.f)));
	
#endif
}

void Bullet::Release()
{
}



int Bullet::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->MoveForward(fTimeDelta * m_fSpeed);

	return 0;
}

void Bullet::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_BULLET, this);
	//ModifyPhysXPos(fTimeDelta);

#ifdef PHYSX
	m_pRigidDynamic->setGlobalPose(MakePxTransform());

	if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_pRigidDynamic, dynamic_cast<Player*>(MainApp::GetInstance()->GetScene()->GetPlayer())->GetPxCapsuleController()->getActor()))
	{
		cout << "Overlap!" << endl;
	}
#endif
	
}

void Bullet::Render(const float& fTimeDelta, int _instanceCount)
{
	//m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	Object::Render(fTimeDelta, _instanceCount);

}



XMFLOAT3 Bullet::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

void Bullet::UpdateSkinnedAnimation(const float fTimeDelta)
{
}

void Bullet::ModifyPhysXPos(const float& fTimeDelta)
{
#ifdef PHYSX
	XMMATRIX matScale, matTrans, matQuat, matWorld;
	XMVECTOR vQuat;

	XMVECTOR vecs, vect;
	XMFLOAT3 xmfScale = dynamic_cast<Transform*>(GetTransController())->GetScale();

	PxRigidDynamic* pRigid = m_pRigidDynamic;
	PxTransform gp = pRigid->getGlobalPose();

	PxMat44 m = PxMat44(gp);

	matWorld = CPhysXMgr::GetInstance()->ToMatrix(m);

	XMMatrixDecompose(&vecs, &vQuat, &vect, dynamic_cast<Transform*>(GetTransController())->GetWorldMatrix());
	matQuat = XMMatrixRotationQuaternion(vQuat);

	matScale = XMMatrixScalingFromVector(XMLoadFloat3(&xmfScale));


	///Gravity
	GravityProgress(fTimeDelta);

	PxVec3 pxVecGp = m.getPosition();
	matTrans = XMMatrixTranslation(pxVecGp.x, pxVecGp.y, pxVecGp.z);


	XMFLOAT4X4 xmf4x4mat;
	XMStoreFloat4x4(&xmf4x4mat, matScale * matQuat * matTrans);

	dynamic_cast<Transform*>(GetTransController())->SetWorld(xmf4x4mat);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(XMFLOAT3(pxVecGp.x, pxVecGp.y, pxVecGp.z));
#endif
}

void Bullet::GravityProgress(const float& fTimeDelta)
{
	if (dynamic_cast<Transform*>(GetTransController())->GetPosition().y <= 0)
		return;
	PxVec3 upDisp = { 0, 1.f ,0 };
	upDisp *= -9.81f * fTimeDelta;
	const PxVec3 disp = upDisp;
	//const PxU32 flags = m_pCapsuleCon->move(disp, 0.0f, fTimeDelta, PxControllerFilters());

	//const PxF32 heightDelta = getHeight(m_fTimeDelta);
	//
	//PxVec3 upDisp = { 0, 1.f ,0 };
	//if (heightDelta != 0.0f) {
	//	upDisp *= heightDelta;
	//}
	//else {
	//	upDisp *= -9.81f * m_fTimeDelta;
	//}
	//const PxVec3 disp = upDisp;
	//
	//const PxU32 flags = m_pCapsuleCon->move(disp, 0.0f, m_fTimeDelta, PxControllerFilters());
	//if (CPhysXMgr::GetInstance()->ToVec3(upDisp).y > 0) {
	//	m_eJumpState = JUMP_UP;
	//}
	//else {
	//	m_eJumpState = JUMP_DOWN;
	//}
	//
	//if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN) // 무언가에 부딪쳤을때
	//{
	//	m_eJumpState = JUMP_LAND;
	//	//stopJump();// 점프를 멈춘다 -> 중력을 안받는다 
	//}
	//if (flags & PxControllerCollisionFlag::eCOLLISION_UP)
	//{
	//	//int a = 0;
	//}
	//
	//if (flags & PxControllerCollisionFlag::eCOLLISION_SIDES)
	//{
	//	//int a = 0;
	//}
}

PxTransform Bullet::MakePxTransform()
{
	XMVECTOR vQuat;
	XMVECTOR vecs, vect;
	XMFLOAT4X4 matWorld;
	XMFLOAT4 xmfQuat;

	XMStoreFloat4x4(&matWorld, dynamic_cast<Transform*>(GetTransController())->GetWorldMatrix());

	XMMatrixDecompose(&vecs, &vQuat, &vect, dynamic_cast<Transform*>(GetTransController())->GetWorldMatrix());
	XMStoreFloat4(&xmfQuat, vQuat);

	PxTransform px;
	px.p.x = matWorld._41;
	px.p.y = matWorld._42;
	px.p.z = matWorld._43;

	px.q.x = xmfQuat.x;
	px.q.y = xmfQuat.y;
	px.q.z = xmfQuat.z;
	px.q.w = xmfQuat.w;

	return px;
}
