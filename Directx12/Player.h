#pragma once
#include "Object.h"

class Geometry;
class Camera;
class InterfaceFSM;
class Component;

class Player :
    public Object
{
public:
    Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer
    ,string _meshName);
    ~Player();
private:
    void    Initialize();
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
    void        SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
public:
    // Get
    Component* GetUpperAniController() { return m_mapComponent["Upper_Animation"]; }
    Component* GetRootAniController() { return m_mapComponent["Root_Animation"]; }
    


protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<SkinnedCB>>         m_SkinnedCB; // 얘는 어쩔수없이 각 캐릭터가 들어야지
    
    Camera*                                     m_pCamera;

    // 컨트롤러
    Component*                                  m_UpperAnimationController;
    Component*                                  m_RootAnimationController;
    Component*                                  m_TransController;
    // FSM
    unique_ptr<InterfaceFSM>                    m_UpperBody;    // 상체
    unique_ptr<InterfaceFSM>                    m_RootBody;     // 하체

};

