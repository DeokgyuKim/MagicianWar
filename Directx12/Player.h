#pragma once
#include "Object.h"
#include "Network.h"

class Geometry;
class Camera;
class InterfaceFSM;
class Component;
class Weapon;

class Player :
    public Object
{
public:
    Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer
        ,string _meshName, XMFLOAT3 pos = XMFLOAT3(10.f, 0.f, 10.f), PlayerInfo playerInfo = {0}, MESH_TYPE meshType = MESH_TYPE::COUNT);
    Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer
        , string _meshName, MESH_TYPE meshType = MESH_TYPE::COUNT, XMFLOAT3 pos = XMFLOAT3(10.f, 0.f, 10.f));
    ~Player();
private:
    void    Initialize(XMFLOAT3 pos);
    void    Release();
    HRESULT BuildConstantBuffer();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

public:

    void UpdateSkinnedCB();
public:
    XMFLOAT3    GetPosition();
    XMFLOAT4X4  GetWorld();
    XMFLOAT3    GetRotate();
    PlayerInfo  GetNetworkInfo() { return m_tNetInfo; }
    int         GetKillCount() { return m_iKillCnt; }
    void        SetPosition(XMFLOAT3 xmfPos);
    void        SetRotate(XMFLOAT3 xmfRotate);
    void        SetWorld(XMFLOAT4X4 world);
    void        SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
    void        SetKillCount(int cnt) { m_iKillCnt = cnt; }
public:
    // Get
    Component* GetUpperAniController();
    Component* GetRootAniController();
    InterfaceFSM* GetRootFSM();
    PxCapsuleController* GetPxCapsuleController() { return m_pCapsuleCon; }
public:
    void        MoveCapsuleController(XMFLOAT3 vSpeed, const float& fTimeDelta);
private:
    void        ModifyPhysXPos(const float& fTimeDelta);
    void        GravityProgress(const float& fTimeDelta);

protected:
    PlayerInfo                                  m_tNetInfo;
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<SkinnedCB>>         m_SkinnedCB; // 얘는 어쩔수없이 각 캐릭터가 들어야지
    
    Camera*                                     m_pCamera = NULL;

    // 컨트롤러
    Component*                                  m_UpperAnimationController;
    Component*                                  m_RootAnimationController;
    Component*                                  m_TransController;
    // FSM
    unique_ptr<InterfaceFSM>                    m_UpperBody;    // 상체
    unique_ptr<InterfaceFSM>                    m_RootBody;     // 하체
    bool m_bAttackEnd;

    //PhysX
    PxCapsuleController* m_pCapsuleCon = nullptr;

    Weapon* m_pWeapon;
    int m_iKillCnt = 0;
};

