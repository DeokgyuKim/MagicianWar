#pragma once
#include "framework.h"
#include "Object.h"

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
    virtual void Render(const float& fTimeDelta) override;

private:
    void    Initialize();
    void    BuildConstantBufferViewMatrix();
    void    BuildConstantBufferProjMatrix();

private:
    ID3D12Device*                       m_pDevice;
    ID3D12GraphicsCommandList*          m_pCmdLst;
    unique_ptr<UploadBuffer<ObjectCB>>	m_ObjectCBView;
    unique_ptr<UploadBuffer<ObjectCB>>	m_ObjectCBProj;


    XMFLOAT4X4                  m_xmmView;
    XMFLOAT4X4                  m_xmmProj;

    XMFLOAT3                    m_xmfPosition;
    XMFLOAT3                    m_xmfTarget;

    XMFLOAT3                    m_xmfRightVec;
    XMFLOAT3                    m_xmfUpVec;
    XMFLOAT3                    m_xmfLookVec;

    float                       m_fRotX, m_fRotY;
    POINT                       m_ptCur, m_ptOld;
    
    bool                        m_bFree;
};

