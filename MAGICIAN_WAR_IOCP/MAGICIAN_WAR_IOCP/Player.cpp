#include "Player.h"
#include "PlayerFSM.h"

Player::Player(int client_num, int room_num)
{
    Initialize(client_num, room_num);
}

Player::~Player()
{
}

void Player::Initialize(int client_num, int room_num)
{
    ZeroMemory(&m_Info, sizeof(PlayerInfo));
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


    CreateCapsuleController();

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
    m_UpperBody->Execute(fTime);
    m_RootBody->Execute(fTime);

    m_Info.PlayerState = m_RootBody->GetState();


}

void Player::LateUpdate(float fTime)
{
    ModifyPhysXPos(fTime);

}

void Player::UpdatePlayerInfo(PlayerInfo* pInfo)
{
    m_Info.PlayerState = m_RootBody->GetState();
    noTransWorldUpdate(pInfo->matWorld);
}

void Player::UpdatePosition()
{
    float globalSpeed = 3.f;
    if (m_keyInput & ctos_KEY_W) {
        if (m_keyInput & ctos_KEY_A) {
            MoveForward((P_MoveForward_Speed / sqrtf(2.f)) * globalSpeed);
            MoveLeft((P_MoveLeft_Speed / sqrtf(2.f)) * globalSpeed);
        }
        else if (m_keyInput & ctos_KEY_D) {
            MoveForward((P_MoveForward_Speed / sqrtf(2.f)) * globalSpeed);
            MoveRight((P_MoveRight_Speed / sqrtf(2.f)) * globalSpeed);
        }
        else {
            MoveForward(P_MoveForward_Speed * globalSpeed);
        }
    }
    else if (m_keyInput & ctos_KEY_S) {
        if (m_keyInput & ctos_KEY_A) {
            MoveBackward((P_MoveBackward_Speed / sqrtf(2.f)) * globalSpeed);
            MoveLeft((P_MoveLeft_Speed / sqrtf(2.f)) * globalSpeed);
        }
        else if (m_keyInput & ctos_KEY_D) {
            MoveBackward((P_MoveBackward_Speed / sqrtf(2.f)) * globalSpeed);
            MoveRight((P_MoveRight_Speed / sqrtf(2.f)) * globalSpeed);
        }
        else {
            MoveBackward(P_MoveBackward_Speed * globalSpeed);
        }
    }
    else if (m_keyInput & ctos_KEY_A) {
        MoveLeft(P_MoveLeft_Speed * globalSpeed);
    }
    else if (m_keyInput & ctos_KEY_D) {
        MoveRight(P_MoveRight_Speed * globalSpeed);
    }
}

void Player::noTransWorldUpdate(XMFLOAT4X4 _world)
{
    m_Info.matWorld._11 = _world._11;
    m_Info.matWorld._12 = _world._12;
    m_Info.matWorld._13 = _world._13;
    m_Info.matWorld._14 = _world._14;

    m_Info.matWorld._21 = _world._21;
    m_Info.matWorld._22 = _world._22;
    m_Info.matWorld._23 = _world._23;
    m_Info.matWorld._24 = _world._24;

    m_Info.matWorld._31 = _world._31;
    m_Info.matWorld._32 = _world._32;
    m_Info.matWorld._33 = _world._33;
    m_Info.matWorld._34 = _world._34;


    m_Info.matWorld._44 = _world._44;
}

void Player::MoveForward(float speed)
{
    XMFLOAT3 look;
    XMFLOAT3 pos = { m_Info.matWorld._41 ,m_Info.matWorld._42 ,m_Info.matWorld._43 };
    memcpy(&look, &m_Info.matWorld._21, sizeof(XMFLOAT3));
    XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
    //XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

    XMFLOAT3 vSpeed;
    XMStoreFloat3(&vSpeed, -XMLoadFloat3(&look) * speed);
    MoveCapsuleController(vSpeed, 1.f);
}

void Player::MoveBackward(float speed)
{
    XMFLOAT3 look;
    XMFLOAT3 pos = { m_Info.matWorld._41 ,m_Info.matWorld._42 ,m_Info.matWorld._43 };
    memcpy(&look, &m_Info.matWorld._21, sizeof(XMFLOAT3));
    XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
    //XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

    XMFLOAT3 vSpeed;
    XMStoreFloat3(&vSpeed, XMLoadFloat3(&look) * speed);
    MoveCapsuleController(vSpeed, 1.f);
}

void Player::MoveLeft(float speed)
{
    XMFLOAT3 left;
    XMFLOAT3 pos = { m_Info.matWorld._41 ,m_Info.matWorld._42 ,m_Info.matWorld._43 };
    memcpy(&left, &m_Info.matWorld._11, sizeof(XMFLOAT3));
    XMStoreFloat3(&left, XMVector3Normalize(XMLoadFloat3(&left)));
    //XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), XMMatrixRotationX(XMConvertToRadians(90.f))));

    XMFLOAT3 vSpeed;
    XMStoreFloat3(&vSpeed, -XMLoadFloat3(&left) * speed);
    MoveCapsuleController(vSpeed, 1.f);
}

void Player::MoveRight(float speed)
{
    XMFLOAT3 right;
    XMFLOAT3 pos = { m_Info.matWorld._41 ,m_Info.matWorld._42 ,m_Info.matWorld._43 };
    memcpy(&right, &m_Info.matWorld._11, sizeof(XMFLOAT3));
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
    XMFLOAT3 pos = { m_Info.matWorld._41 ,m_Info.matWorld._42 ,m_Info.matWorld._43 };
    memcpy(&up, &m_Info.matWorld._31, sizeof(XMFLOAT3));
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
    xmmWorld = XMLoadFloat4x4(&m_Info.matWorld) * xmmTrans;
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
    m_keyInput = _key;
}


void Player::setPosition(XMFLOAT3 pos)
{
    m_Info.xmfPosition = pos;

    m_Info.matWorld._41 = pos.x;
    m_Info.matWorld._42 = pos.y;
    m_Info.matWorld._43 = pos.z;
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

    XMMatrixDecompose(&vecs, &vQuat, &vect, XMLoadFloat4x4(&m_Info.matWorld));
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

    XMStoreFloat4x4(&m_Info.matWorld, matScale * matQuat * matTrans * matOffset);
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
