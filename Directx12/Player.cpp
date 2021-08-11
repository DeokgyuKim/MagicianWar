#include "Player.h"

#include "MainApp.h"
#include "Core.h"
#include "Scene.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "Bullet.h"
#include "Weapon.h"
#include "Network.h"

// Component
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Animation.h"
#include "KeyMgr.h"

// FSM
#include "PlayerFSM.h"

#include "Network.h"


Player::Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string _meshName, XMFLOAT3 pos,
	PlayerInfo playerInfo, MESH_TYPE meshType)
	:Object(_meshName, meshType)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;

	m_tNetInfo = playerInfo;

	m_strMeshType = meshType;
	Initialize(pos);
	m_bAttackEnd = false;

}

Player::Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string _meshName, MESH_TYPE meshType, XMFLOAT3 pos)
	:Object(_meshName, meshType)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;

	Initialize(pos);
}

Player::~Player()
{
}

void Player::Initialize(XMFLOAT3 pos)
{

	Core::GetInstance()->CmdLstReset();
	BuildConstantBuffer();

	Component* pComponent = new Transform(XMFLOAT3(0.01f, 0.01f, 0.01f), XMFLOAT3(0.f, 0.f, 0.f), pos);
	//Component* pComponent = new Transform(XMFLOAT3(0.1f, 0.1f, 0.1f), XMFLOAT3(0.f, 0.f, 0.f), pos);
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), m_strInstName);
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom(m_strInstName);
	m_mapComponent["Material"] = pComponent;
	pComponent = new AnimationCom(m_strInstName);
	m_mapComponent["Upper_Animation"] = pComponent;
	pComponent = new AnimationCom(m_strInstName);
	m_mapComponent["Root_Animation"] = pComponent;

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetMeshRotate(XMFLOAT3(-90.f, 0.f, 0.f));
	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();

#ifndef NETWORK
	// FSM 만들기
	m_UpperBody = make_unique<PlayerFSM>(this, BoneType::UPPER_BONE); // ( player , BoneType )
	m_RootBody = make_unique<PlayerFSM>(this, BoneType::ROOT_BONE);
#endif

	m_strTextureName = m_strInstName;

	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_pWeapon = new Weapon(m_pDevice, m_pCmdLst, m_pRenderer, "weapon", "weapon", this, 20, XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(-157.f, 177.f, -101.f), XMFLOAT3(-50.f, -6.f, 44.f));


#ifdef PHYSX
	CPhysXMgr::GetInstance()->m_PlayerController = m_pCapsuleCon = CPhysXMgr::GetInstance()->
		CreateCapsuleController(this, pos, 0.5f, 0.5f, true);
	m_pCapsuleCon->getActor()->setName("Player");
	m_pCapsuleCon->getActor()->setMass(20.f);
#endif
}

void Player::Release()
{
}

HRESULT Player::BuildConstantBuffer()
{
	m_SkinnedCB = make_unique<UploadBuffer<SkinnedCB>>(m_pDevice, 1, true);

	return S_OK;
}

int Player::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);
	if (m_pCamera != NULL)
	{
		if (m_pCamera->GetMode() == CAMERA_MODE::CAMERA_THIRD)
		{
			XMFLOAT3 xmfRotate = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();
			float cameraY = m_pCamera->GetRotY();
#ifdef NETWORK
			if (xmfRotate.y != cameraY) {
				Network::GetInstance()->SendCameraUpdate(cameraY);
			}
#else
			xmfRotate.y = m_pCamera->GetRotY();
			dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(xmfRotate);
#endif

		}
	}
#ifndef NETWORK
	m_UpperBody->Execute(fTimeDelta);
	m_RootBody->Execute(fTimeDelta);
#endif

	m_pWeapon->Update(fTimeDelta);

	return 0;
}

void Player::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	//UpdateObjectCB();
	//UpdateSkinnedCB();

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_DYNAMIC, this);

	ModifyPhysXPos(fTimeDelta);

	//if (m_pCamera == NULL)
	//{
#ifdef NETWORK
	STOC_PlayerInfo info = Network::GetInstance()->GetRecvPlayerInfo(m_tNetInfo.Client_Num);
	XMFLOAT3 rotate = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();
	rotate.y = info.playerInfo.CameraY;
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(info.playerInfo.xmfPosition);
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(rotate);
	dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->ChangeAnimation(SCint(info.Upper_eAnimType));
	dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->ChangeAnimation(SCint(info.Root_eAnimType));
	if (m_bAttackEnd != dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->GetAttackEnd()) {
		m_bAttackEnd = dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->GetAttackEnd();
		Network::GetInstance()->CallEvent(EVENT_STAGE_PLAYER_ANIMATE, 1, m_bAttackEnd);
	}
#endif
	//}
	m_pWeapon->LateUpdate(fTimeDelta);
}

void Player::Render(const float& fTimeDelta, int _instanceCount)
{
	//cout << GetWorld()._41 << ", " << GetWorld()._42
	//m_pCmdLst->SetGraphicsRootShaderResourceView(0, InstanceMgr::GetInstnace()->m_InstanceCBs[m_strMeshName]->Resource()->GetGPUVirtualAddress());	// obj
	//m_pCmdLst->SetGraphicsRootConstantBufferView(11, m_SkinnedCB->Resource()->GetGPUVirtualAddress());	// skinned
	Object::Render(fTimeDelta, _instanceCount);

	//m_pWeapon->Render(fTimeDelta, 1);

}



void Player::UpdateSkinnedCB()
{
	//// SkinnedCB // 이거 애니메이션 붙이기 전까지 붙이면 안뜸 당연하지 쓰레기값가지고 계산되니까
	//SkinnedCB SkinnedCB;
	//
	//SkinnedCB.BoneTransforms[0] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[0]; // Root 뼈대 == 하체 중심
	//SkinnedCB.BoneTransforms[1] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[1]; // Root 뼈대 == 하체 중심
	//for (int upper = 2; upper < 27; ++upper) { // 상체
	//	SkinnedCB.BoneTransforms[upper] = dynamic_cast<AnimationCom*>(m_mapComponent["Upper_Animation"])->GetSkinnedModellnst()->FinalTransforms[upper];
	//}
	//for (int Root = 27; Root < 33; ++Root) { // 하체
	//	SkinnedCB.BoneTransforms[Root] = dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms[Root];
	//}
	////copy(begin(dynamic_cast<AnimationCom*>(m_mapComponent["Animation"])->GetSkinnedModellnst()->FinalTransforms),
	////	end(dynamic_cast<AnimationCom*>(m_mapComponent["Root_Animation"])->GetSkinnedModellnst()->FinalTransforms),
	////	&SkinnedCB.BoneTransforms[0]);
	//
	//m_SkinnedCB->CopyData(0, SkinnedCB);
}

XMFLOAT3 Player::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

XMFLOAT4X4 Player::GetWorld()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix());
	return world;
}

XMFLOAT3 Player::GetRotate()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();
}

void Player::SetPosition(XMFLOAT3 xmfPos)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(xmfPos);
}

void Player::SetRotate(XMFLOAT3 xmfRotate)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(xmfRotate);
}

void Player::SetWorld(XMFLOAT4X4 world)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetWorld(world);
}

Component* Player::GetUpperAniController()
{
	auto iter = m_mapComponent.find("Upper_Animation");
	if (iter == m_mapComponent.end())
		return nullptr;

	return (*iter).second;
}

Component* Player::GetRootAniController()
{
	auto iter = m_mapComponent.find("Root_Animation");
	if (iter == m_mapComponent.end())
		return nullptr;

	return (*iter).second;
}

InterfaceFSM* Player::GetRootFSM()
{
	if (m_RootBody == nullptr) // 
		return nullptr;
	return m_RootBody.get();
}

void Player::MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta)
{
	if (m_pCapsuleCon == NULL)
		return;
	PxControllerCollisionFlags colflag = m_pCapsuleCon->move(CPhysXMgr::GetInstance()->ToPxVec3(vSpeed) * fTimeDelta, 0.f, fTimeDelta, PxControllerFilters());
}

void Player::ModifyPhysXPos(const float& fTimeDelta)
{
#ifdef PHYSX
	XMMATRIX matScale, matTrans, matQuat, matWorld;
	XMVECTOR vQuat;

	XMVECTOR vecs, vect;
	XMFLOAT3 xmfScale = dynamic_cast<Transform*>(GetTransController())->GetScale();

	PxRigidDynamic* pRigid = m_pCapsuleCon->getActor();
	PxTransform gp = pRigid->getGlobalPose();


	cout << "Modify" << gp.p.x << ", " << gp.p.y << ", " << gp.p.z << endl;

	PxMat44 m = PxMat44(gp);

	matWorld = CPhysXMgr::GetInstance()->ToMatrix(m);

	XMMatrixDecompose(&vecs, &vQuat, &vect, dynamic_cast<Transform*>(GetTransController())->GetWorldMatrix());
	matQuat = XMMatrixRotationQuaternion(vQuat);

	matScale = XMMatrixScalingFromVector(XMLoadFloat3(&xmfScale));


	///Gravity
	GravityProgress(fTimeDelta);

	PxExtendedVec3 GetPosition = m_pCapsuleCon->getPosition();
	PxVec3 pxVecGp;
	pxVecGp.x = float(GetPosition.x);
	pxVecGp.y = float(GetPosition.y) - 0.85f;
	pxVecGp.z = float(GetPosition.z);
	matTrans = XMMatrixTranslation(pxVecGp.x, pxVecGp.y, pxVecGp.z);

	//matOffset = XMMatrixTranslation(0.f, -0.45f, 0.f);


	XMFLOAT4X4 xmf4x4mat;
	XMStoreFloat4x4(&xmf4x4mat, matScale * matQuat * matTrans);

	dynamic_cast<Transform*>(GetTransController())->SetWorld(xmf4x4mat);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(XMFLOAT3(GetPosition.x, GetPosition.y, GetPosition.z));

	XMFLOAT4 xmfQuat;
	XMStoreFloat4(&xmfQuat, vQuat);

	PxTransform px;
	px.p = pxVecGp;
	px.q.x = xmfQuat.x;
	px.q.y = xmfQuat.y;
	px.q.z = xmfQuat.z;
	px.q.w = xmfQuat.w;

	pRigid->setGlobalPose(px);

#endif
}

void Player::GravityProgress(const float& fTimeDelta)
{
	if (dynamic_cast<Transform*>(GetTransController())->GetPosition().y <= 0)
		return;
	PxVec3 upDisp = { 0, 1.f ,0 };
	upDisp *= -9.81f * fTimeDelta;
	const PxVec3 disp = upDisp;
	const PxU32 flags = m_pCapsuleCon->move(disp, 0.0f, fTimeDelta, PxControllerFilters());

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


