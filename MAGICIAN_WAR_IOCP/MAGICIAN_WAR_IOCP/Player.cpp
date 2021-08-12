#include "Player.h"
#include "PlayerFSM.h"

Player::Player(int client_num, int room_num)
{
	Initialize(client_num, room_num);
}

Player::~Player()
{
	cout << "플레이어 데이터 지우기\n";
	if (m_pCapsuleCon != NULL)
	{
		CPhysXMgr::GetInstance()->gScene->removeActor(*m_pCapsuleCon->getActor());
		m_pCapsuleCon->release();
	}

}

void Player::Initialize(int client_num, int room_num)
{
	ZeroMemory(&m_Info, sizeof(PlayerInfo));

	m_Camera.CamX = 0.f;
	m_Camera.CamY = 0.f;
	m_xmfScale = XMFLOAT3(0.01f, 0.01f, 0.01f);
	m_xmfRotate = XMFLOAT3(0.f, 0.f, 0.f);
	m_xmfMeshRotate = XMFLOAT3(-90.f, 0.f, 0.f);
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());
	

	m_Info.PlayerState = STATE_IDLE;
	m_Info.CharacterType = ELEMENT_FIRE;
	m_Info.Client_Num = client_num;
	m_Info.isRoom_Host = false;
	m_Info.Room_Num = room_num;
	m_Info.iHp = 100;
	m_keyInput = 0;
	m_Ready = false;
	m_Bullet = 0;
	m_Room_JoinState = false;

	m_UpperBody = make_unique<PlayerFSM>(this, BONE_UPPER); // ( player , BoneType )
	m_RootBody = make_unique<PlayerFSM>(this, BONE_ROOT);

	m_Root_eAnimType = ANIM_IDLE;
	m_Root_fAnimTime = 0.f;
	m_Root_eAnimBlendType = ANIM_IDLE;
	m_Root_fWeight = 0.f;

	m_Upper_eAnimType = ANIM_IDLE;
	m_Upper_fAnimTime = 0.f;
	m_Upper_eAnimBlendType = ANIM_IDLE;
	m_Upper_fWeight = 0.f;
	m_LateInit = false;



}

void Player::LateInit()
{
	if (m_LateInit) return;

	CreateCapsuleController();



	m_LateInit = true;
}

void Player::ReInit()
{
	m_Info.PlayerState = STATE_IDLE;
	m_Ready = false;
	m_Root_eAnimType = ANIM_IDLE;
	m_Root_fAnimTime = 0.f;
	m_Root_eAnimBlendType = ANIM_IDLE;
	m_Root_fWeight = 0.f;

	m_Upper_eAnimType = ANIM_IDLE;
	m_Upper_fAnimTime = 0.f;
	m_Upper_eAnimBlendType = ANIM_IDLE;
	m_Upper_fWeight = 0.f;

	m_Info.iHp = 100;
	m_keyInput = 0;

	m_Bullet = 0;



}

void Player::Update(float fTime)
{
	LateInit();


	m_UpperBody->Execute(fTime);
	m_RootBody->Execute(fTime);

}

void Player::LateUpdate(float fTime)
{
	UpdatePlayerInfo();
	ModifyPhysXPos(fTime);

}

void Player::UpdatePlayerInfo()
{
	m_Info.PlayerState = m_RootBody->GetState();

	XMMATRIX scale, rotateX, rotateY, rotateZ, transform;

	scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_xmfScale));

	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x + m_xmfMeshRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y + m_xmfMeshRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z + m_xmfMeshRotate.z));

	transform = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Info.xmfPosition));

	XMMATRIX world = scale * rotateX * rotateY * rotateZ * transform;

	XMStoreFloat4x4(&m_xmmWorld, world);

}





void Player::MoveForward(float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { m_xmmWorld._41 ,m_xmmWorld._42 ,m_xmmWorld._43 };
	memcpy(&look, &m_xmmWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, -XMLoadFloat3(&look) * speed);
	MoveCapsuleController(vSpeed, 1.f);
}

void Player::MoveBackward(float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { m_xmmWorld._41 ,m_xmmWorld._42 ,m_xmmWorld._43 };
	memcpy(&look, &m_xmmWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, XMLoadFloat3(&look) * speed);
	MoveCapsuleController(vSpeed, 1.f);
}

void Player::MoveLeft(float speed)
{
	XMFLOAT3 left;
	XMFLOAT3 pos = { m_xmmWorld._41 ,m_xmmWorld._42 ,m_xmmWorld._43 };
	memcpy(&left, &m_xmmWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&left, XMVector3Normalize(XMLoadFloat3(&left)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, -XMLoadFloat3(&left) * speed);
	MoveCapsuleController(vSpeed, 1.f);
}

void Player::MoveRight(float speed)
{
	XMFLOAT3 right;
	XMFLOAT3 pos = { m_xmmWorld._41 ,m_xmmWorld._42 ,m_xmmWorld._43 };
	memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, XMLoadFloat3(&right) * speed);
	MoveCapsuleController(vSpeed, 1.f);
}

bool Player::Jump(float fTime)
{
	m_fAccTime += fTime;
	float fY = m_fJumpSpeedY * fTime;

	m_fJumpSpeedY -= fTime;
	m_fJumpSpeedY = max(0.f, m_fJumpSpeedY);


	XMFLOAT3 up;
	XMFLOAT3 pos = { m_xmmWorld._41 ,m_xmmWorld._42 ,m_xmmWorld._43 };
	memcpy(&up, &m_xmmWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&up, XMVector3Normalize(XMLoadFloat3(&up)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, XMLoadFloat3(&up) * m_fJumpSpeedY);
	MoveCapsuleController(vSpeed, 1.f);

	//for (auto obj : StaticObjects)
	//    if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_pCapsuleCon->getActor(), obj))
	//        return true;
	return false;
}


XMFLOAT4X4 Player::getBulletStartWorld()
{
	XMMATRIX xmmWorld, xmmTrans;
	xmmTrans = XMMatrixTranslation(0.f, 1.f, 0.f);
	xmmWorld = XMLoadFloat4x4(&m_xmmWorld) * xmmTrans;
	XMFLOAT4X4 xmfWorld;
	XMStoreFloat4x4(&xmfWorld, xmmWorld);

	return xmfWorld;
}

int Player::getState()
{
	return m_RootBody->GetState(); 
}

InterfaceFSM* Player::GetRootFSM()
{
	if (m_RootBody == nullptr)
		return nullptr;
	return m_RootBody.get();
}

InterfaceFSM* Player::GetUpperFSM()
{
	if (m_UpperBody == nullptr)
		return nullptr;
	return m_UpperBody.get();
}

void Player::setDamage(int damage)
{
	m_Info.iHp -= damage;
}

void Player::setKeyInput(DWORD _key)
{
	m_keyInput = _key;
}


void Player::setPosition(XMFLOAT3 pos)
{
	m_Info.xmfPosition = pos;

	m_xmmWorld._41 = pos.x;
	m_xmmWorld._42 = pos.y;
	m_xmmWorld._43 = pos.z;
	
	CreateCapsuleController();
}

void Player::setPlayerInitPos(XMFLOAT3 pos)
{
	m_Info.xmfPosition = pos;
}

void Player::ChangeUpperAnimation(int _Ani)
{
	int nextAni = _Ani;
	if (m_Upper_eAnimType != nextAni)
		m_Upper_eAnimType = nextAni;
}

void Player::ChangeRootAnimation(int _Ani)
{
	int nextAni = _Ani;
	if (m_Root_eAnimType != nextAni)
		m_Root_eAnimType = nextAni;
}

void Player::setJump(bool bJump)
{
	m_bJump = bJump;
	float m_fJumpSpeedY = 2.f;
	float m_fAccTime = 0.f;
}

void Player::setCharacterType(unsigned char _type)
{
	m_Info.CharacterType = _type;
}

void Player::setHost(bool bHost)
{
	if (bHost) { // 호스트는 항상 레디 상태
		m_Ready = true;
	}
}

void Player::SetRotate(XMFLOAT3 xmfRotate)
{
	m_xmfRotate = xmfRotate;
	m_Info.CameraY = m_xmfRotate.y;
}

void Player::CreateCapsuleController()
{
	XMFLOAT3 xmfpos = XMFLOAT3(0.f, 0.85f, 0.f);
	XMStoreFloat3(&xmfpos, XMLoadFloat3(&xmfpos) + XMLoadFloat3(&m_Info.xmfPosition));

	if (m_pCapsuleCon != NULL)
	{
		CPhysXMgr::GetInstance()->gScene->removeActor(*m_pCapsuleCon->getActor());
		m_pCapsuleCon->release();
	}


	CPhysXMgr::GetInstance()->m_PlayerController = m_pCapsuleCon = CPhysXMgr::GetInstance()->
		CreateCapsuleController(m_Info.Client_Num, xmfpos, 0.5f, 0.5f, true);
	m_pCapsuleCon->getActor()->setName("Player");
	m_pCapsuleCon->getActor()->setMass(20.f);

}

void Player::ModifyPhysXPos(const float& fTimeDelta)
{
	XMMATRIX matScale, matTrans, matQuat, matWorl, matOffset;
	XMVECTOR vQuat;

	XMVECTOR vecs, vect;
	XMFLOAT3 xmfScale = XMFLOAT3(0.01f, 0.01f, 0.01f);

	PxRigidDynamic* pRigid = m_pCapsuleCon->getActor();
	PxTransform gp = pRigid->getGlobalPose();

	PxMat44 m = PxMat44(gp);

	matWorl = CPhysXMgr::GetInstance()->ToMatrix(m);

	XMMatrixDecompose(&vecs, &vQuat, &vect, XMLoadFloat4x4(&m_xmmWorld));
	matQuat = XMMatrixRotationQuaternion(vQuat);

	matScale = XMMatrixScalingFromVector(XMLoadFloat3(&xmfScale));


	///Gravity
	GravityProgress(fTimeDelta);

	PxExtendedVec3 GetPosition = m_pCapsuleCon->getPosition();
	PxVec3 pxVecGp;
	pxVecGp.x = float(GetPosition.x);
	pxVecGp.y = float(GetPosition.y);
	pxVecGp.z = float(GetPosition.z);
	matTrans = XMMatrixTranslation(pxVecGp.x, pxVecGp.y, pxVecGp.z);

	if (m_Info.CharacterType != ELEMENT_DARKNESS)
		matOffset = XMMatrixTranslation(0.f, -0.85f, 0.f);
	else
		matOffset = XMMatrixTranslation(0.f, -0.45f, 0.f);



	XMStoreFloat4x4(&m_matRealWorld, matScale * matQuat * matTrans);

	XMStoreFloat4x4(&m_xmmWorld, matScale * matQuat * matTrans * matOffset);

	memcpy(&m_Info.xmfPosition, &m_xmmWorld._41, sizeof(XMFLOAT3));
}

void Player::GravityProgress(const float& fTimeDelta)
{
	if (m_matRealWorld._42 <= 0.f)
		return;
	PxVec3 upDisp = { 0, 1.f ,0 };
	upDisp *= -0.98f * fTimeDelta;
	const PxVec3 disp = upDisp;
	const PxU32 flags = m_pCapsuleCon->move(disp, 0.0f, fTimeDelta, PxControllerFilters());
}

void Player::MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta)
{
	if (m_pCapsuleCon == NULL)
		return;
	PxControllerCollisionFlags colflag = m_pCapsuleCon->move(CPhysXMgr::GetInstance()->ToPxVec3(vSpeed) * fTimeDelta, 0.f, fTimeDelta, PxControllerFilters());
}
