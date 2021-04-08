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
    void UpdateObjectCB();
    void UpdateSkinnedCB();
public:
    XMFLOAT3    GetPosition();
    void        SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
public:
    // Get
    Component* GetUpperAniController() { return m_mapComponent["Upper_Animation"]; }
    Component* GetRootAniController() { return m_mapComponent["Root_Animation"]; }
    Component* GetTransController() { return m_mapComponent["Transform"]; }


protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB; // �̰� ����
    unique_ptr<UploadBuffer<SkinnedCB>>         m_SkinnedCB; // ��� ��¿������ �� ĳ���Ͱ� ������
    
    Camera*                                     m_pCamera;

    // ��Ʈ�ѷ�
    Component*                                  m_UpperAnimationController;
    Component*                                  m_RootAnimationController;
    Component*                                  m_TransController;
    // FSM
    unique_ptr<InterfaceFSM>                    m_UpperBody;    // ��ü
    unique_ptr<InterfaceFSM>                    m_RootBody;     // ��ü

};

