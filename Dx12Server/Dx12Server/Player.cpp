#include "Player.h"

// FSM
#include "PlayerFSM.h"

Player::Player()
{
	ZeroMemory(&Info, sizeof(Info));
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	ePlayerState = PLAYER_STATE::NONE;
	
	Root_eAnimType = ANIMATION_TYPE::NONE;
	Root_fAnimTime = 0.f;
	Root_eAnimBlendType = ANIMATION_TYPE::NONE;
	Root_fWeight = 0.f;

	Upper_eAnimType = ANIMATION_TYPE::NONE;
	Upper_fAnimTime = 0.f;
	Upper_eAnimBlendType = ANIMATION_TYPE::NONE;
	Upper_fWeight = 0.f;

	m_isConnected = false;

	//m_bAttackEnd = false;

	m_keyInput = 0;
	m_iHp = 100;
	m_Bullet = 0;
}

Player::~Player()
{
	// 소켓 닫고 서버와의 연결 끊기
	closesocket(Info.socket);
	m_isConnected = false;
}

void Player::Initialize(SOCKET& sock, int ID) // sock 와 id를 받아서 초기화
{
	Info.socket = sock;
	Info.info.dwPlayerNum = ID;
	Info.info.dwTeamNum = ID;
	Info.info.iHp = 100;
	
	m_Ready = false; // 초기 unReady
	m_isConnected = true; // 서버와의 연결


	recv_start_ptr = recvBuf;
	packet_start_ptr = recvBuf;

	// FSM 만들기
	m_UpperBody = make_unique<PlayerFSM>(this, BoneType::UPPER_BONE); // ( player , BoneType )
	m_RootBody = make_unique<PlayerFSM>(this, BoneType::ROOT_BONE);
	
}

void Player::Update(float fTime)
{
	// 여기서 이동에 관한 업데이트 하면 되겠지..
	//UpdatePosition();
	
	m_UpperBody->Execute(fTime);
	m_RootBody->Execute(fTime);

	ePlayerState = m_RootBody->GetState();


	//cout << m_matRealWorld._42 << endl;
}

void Player::UpdatePlayerInfo(CTOS_PlayerInfo* pInfo)
{
	m_mutex.lock();
	//	
	ePlayerState = m_RootBody->GetState(); // 플레이어 상태를 갱신해준다.
	m_bAttackEnd = pInfo->bAttackEnd;
	noTransWorldUpdate(pInfo->matWorld);
	m_InstanceName = pInfo->InstanceName;
	m_ID = pInfo->id;
	//
	m_mutex.unlock();
}

void Player::UpdatePosition()
{
	float globalSpeed = 3.f;
	if (m_keyInput & 0x0001) {
		if (m_keyInput & 0x0002) {
			MoveForward((M_MoveForward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveLeft((M_MoveLeft_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else if (m_keyInput & 0x0008) {
			MoveForward((M_MoveForward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveRight((M_MoveRight_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else {
			MoveForward(M_MoveForward_Speed * globalSpeed);
		}
	}
	else if (m_keyInput & 0x0004) {
		if (m_keyInput & 0x0002) {
			MoveBackward((M_MoveBackward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveLeft((M_MoveLeft_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else if (m_keyInput & 0x0008) {
			MoveBackward((M_MoveBackward_Speed / sqrtf(2.f)) * globalSpeed);
			MoveRight((M_MoveRight_Speed / sqrtf(2.f)) * globalSpeed);
		}
		else {
			MoveBackward(M_MoveBackward_Speed * globalSpeed);
		}
	}
	else if (m_keyInput & 0x0002) {
		MoveLeft(M_MoveLeft_Speed * globalSpeed);
	}
	else if (m_keyInput & 0x0008) {
		MoveRight(M_MoveRight_Speed * globalSpeed);
	}
}

void Player::noTransWorldUpdate(XMFLOAT4X4 _world)
{
	matWorld._11 = _world._11;
	matWorld._12 = _world._12;
	matWorld._13 = _world._13;
	matWorld._14 = _world._14;

	matWorld._21 = _world._21;
	matWorld._22 = _world._22;
	matWorld._23 = _world._23;
	matWorld._24 = _world._24;

	matWorld._31 = _world._31;
	matWorld._32 = _world._32;
	matWorld._33 = _world._33;
	matWorld._34 = _world._34;


	matWorld._44 = _world._44;

}

void Player::MoveForward(float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&look, &matWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, -XMLoadFloat3(&look) * speed);
	MoveCapsuleController(vSpeed, 1.f);

	//XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&look) * speed);
	//matWorld._41 = pos.x;
	//matWorld._42 = pos.y;
	//matWorld._43 = pos.z;
}

void Player::MoveBackward(float speed)
{
	XMFLOAT3 look;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&look, &matWorld._21, sizeof(XMFLOAT3));
	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, XMLoadFloat3(&look) * speed);
	MoveCapsuleController(vSpeed, 1.f);

	//XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&look) * speed);
	//matWorld._41 = pos.x;
	//matWorld._42 = pos.y;
	//matWorld._43 = pos.z;
}

void Player::MoveLeft(float speed)
{
	XMFLOAT3 left;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&left, &matWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&left, XMVector3Normalize(XMLoadFloat3(&left)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, -XMLoadFloat3(&left) * speed);
	MoveCapsuleController(vSpeed, 1.f);

	//XMStoreFloat3(&pos, XMLoadFloat3(&pos) - XMLoadFloat3(&left) * speed);
	//matWorld._41 = pos.x;
	//matWorld._42 = pos.y;
	//matWorld._43 = pos.z;
}

void Player::MoveRight(float speed)
{
	XMFLOAT3 right;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&right, &matWorld._11, sizeof(XMFLOAT3));
	XMStoreFloat3(&right, XMVector3Normalize(XMLoadFloat3(&right)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, XMLoadFloat3(&right) * speed);
	MoveCapsuleController(vSpeed, 1.f);

	//XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&right) * speed);
	//matWorld._41 = pos.x;
	//matWorld._42 = pos.y;
	//matWorld._43 = pos.z;
}

bool Player::Jump(float fTime)
{
	m_fAccTime += fTime;
	float fY = m_fJumpSpeedY * fTime;

	m_fJumpSpeedY -= fTime;
	m_fJumpSpeedY = max(0.f, m_fJumpSpeedY);


	XMFLOAT3 up;
	XMFLOAT3 pos = { matWorld._41 ,matWorld._42 ,matWorld._43 };
	memcpy(&up, &matWorld._31, sizeof(XMFLOAT3));
	XMStoreFloat3(&up, XMVector3Normalize(XMLoadFloat3(&up)));
	//XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

	XMFLOAT3 vSpeed;
	XMStoreFloat3(&vSpeed, XMLoadFloat3(&up) * m_fJumpSpeedY);
	MoveCapsuleController(vSpeed, 1.f);

	for (auto obj : StaticObjects)
		if (CPhysXMgr::GetInstance()->OverlapBetweenTwoObject(m_pCapsuleCon->getActor(), obj))
			return true;
	return false;
}

int Player::recvPacket()
{
	return recv(Info.socket, recv_start_ptr, MAX_BUFFER, 0);
}

int Player::sendPacket(void* _packet, int _size)
{
	return send(Info.socket, (char*)(_packet), _size, 0);
}

XMFLOAT4X4 Player::getBulletStartWorld()
{
	XMMATRIX xmmWorld, xmmTrans;
	xmmTrans = XMMatrixTranslation(0.f, 1.f, 0.f);
	xmmWorld = XMLoadFloat4x4(&matWorld) * xmmTrans;
	XMFLOAT4X4 xmfWorld;
	XMStoreFloat4x4(&xmfWorld, xmmWorld);

	return xmfWorld;
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

void Player::setKeyInput(DWORD _key)
{
	m_mutex.lock();
	m_keyInput = _key;
	m_mutex.unlock();
}

void Player::setReady(DWORD _ready)
{
	m_mutex.lock();
	m_Ready = _ready;
	m_mutex.unlock();
}

void Player::setPosition(XMFLOAT3 pos)
{
	m_mutex.lock();
	matWorld._41 = pos.x;
	matWorld._42 = pos.y;
	matWorld._43 = pos.z;
	m_mutex.unlock();
}

void Player::setPlayerInitPos(XMFLOAT3 pos)
{
	m_mutex.lock();
	Info.info.xmfPosition = pos;
	m_mutex.unlock();
}


void Player::ChangeUpperAnimation(int _Ani)
{
	ANIMATION_TYPE nextAni = static_cast<ANIMATION_TYPE>(_Ani);
	if(Upper_eAnimType != nextAni)
		Upper_eAnimType = nextAni;
}

void Player::ChangeRootAnimation(int _Ani)
{
	ANIMATION_TYPE nextAni = static_cast<ANIMATION_TYPE>(_Ani);
	if (Root_eAnimType != nextAni)
		Root_eAnimType = nextAni;
}

void Player::setJump(bool bJump)
{ 
	m_bJump = bJump;
	float m_fJumpSpeedY = 2.f;
	float m_fAccTime = 0.f;
}

void Player::setCharacterType(unsigned char _type)
{
	m_mutex.lock();
	m_CharacterType = _type;
	m_mutex.unlock();
}

void Player::CreateCapsuleController()
{
	m_mutex.lock();
	CPhysXMgr::GetInstance()->m_PlayerController = m_pCapsuleCon = CPhysXMgr::GetInstance()->
		CreateCapsuleController(Info.info.dwPlayerNum, Info.info.xmfPosition, 0.5f, 0.5f, true);
	m_pCapsuleCon->getActor()->setName("Player");
	m_pCapsuleCon->getActor()->setMass(20.f);
	m_mutex.unlock();
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

	XMMatrixDecompose(&vecs, &vQuat, &vect, XMLoadFloat4x4(&matWorld));
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

	matOffset = XMMatrixTranslation(0.f, -0.85f, 0.f);

	
	XMStoreFloat4x4(&m_matRealWorld, matScale * matQuat * matTrans);
	XMStoreFloat4x4(&matWorld, matScale * matQuat * matTrans * matOffset);
}

void Player::GravityProgress(const float& fTimeDelta)
{
	if (m_matRealWorld._42 <= 0.f)
		return;
	PxVec3 upDisp = { 0, 1.f ,0 };
	upDisp *= -9.81f * fTimeDelta;
	const PxVec3 disp = upDisp;
	const PxU32 flags = m_pCapsuleCon->move(disp, 0.0f, fTimeDelta, PxControllerFilters());
}

void Player::MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta)
{
	if (m_pCapsuleCon == NULL)
		return;
	PxControllerCollisionFlags colflag = m_pCapsuleCon->move(CPhysXMgr::GetInstance()->ToPxVec3(vSpeed) * fTimeDelta, 0.f, fTimeDelta, PxControllerFilters());

}

