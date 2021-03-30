#pragma once
#include "Object.h"

class Geometry;
class Camera;
class AnimationController;
class InterfaceFSM;

class Player :
    public Object
{
public:
    Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~Player();
private:
    void    Initialize();
    void    Release();
    HRESULT BuildConstantBuffer();
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;

public:
    XMFLOAT3    GetPosition();
    void        SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

private:
    void UpdateSkinnedAnimation(const float fTimeDelta);
    void KeyInput();    // Ű �Է�
    void KeyPress();     // Ű ����
    void KeyDown();
    void KeyUp();            // Ű ��
protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB;
    unique_ptr<UploadBuffer<SkinnedCB>>         m_SkinnedCB;
    unique_ptr<UploadBuffer<MaterialCB>>        m_MaterialCB;
    
    Camera*                                     m_pCamera;

    // ��Ʈ�ѷ�
    //unique_ptr<AnimationController>             m_AnimationController;
    // FSM
    unique_ptr<InterfaceFSM>                    m_UpperBody;    // ��ü
    unique_ptr<InterfaceFSM>                    m_RootBody;     // ��ü

};

