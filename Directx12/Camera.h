#pragma once
#include "framework.h"
#include "Object.h"

class Player;

class Camera :
    public Object
{
public:
    Camera(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~Camera();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

    void        SetPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }
    void        SetMode(CAMERA_MODE eMode);
    CAMERA_MODE GetMode() { return m_eCamMode; }
    float       GetRotX() { return m_fRotY; }
    float       GetRotY() { return m_fRotX; }

    XMFLOAT3        GetLookVector() { return m_xmfLookVec; }

    XMFLOAT3    GetPosition() { return m_xmfPosition; }
    XMFLOAT4X4  GetViewMatrix() { return m_xmmView; }
    XMFLOAT4X4  GetProjMatrix() { return m_xmmProj; }

    void ReSetCameraRot(float rotate) { m_fRotY = rotate; m_fRotX = rotate; }
    void CameraRotation();

private:
    void    Initialize();
    void    BuildConstantBuffer();
 

private:
    ID3D12Device*                       m_pDevice;
    ID3D12GraphicsCommandList*          m_pCmdLst;
    unique_ptr<UploadBuffer<CameraCB>>  m_ObjectCB;


    XMFLOAT4X4                  m_xmmView;
    XMFLOAT4X4                  m_xmmProj;

    XMFLOAT3                    m_xmfPosition;
    XMFLOAT3                    m_xmfTarget;

    XMFLOAT3                    m_xmfRightVec;
    XMFLOAT3                    m_xmfUpVec;
    XMFLOAT3                    m_xmfLookVec;

    float                       m_fOffset;

    float                       m_fRotX, m_fRotY;
    POINT                       m_ptCur, m_ptOld;
    
    CAMERA_MODE                 m_eCamMode = CAMERA_MODE::CAMERA_NONE;

    Player*                     m_pPlayer;
    float                       m_fTime = 0.f;
    PxRigidBody*                m_pRigidBody = NULL;
};

