#pragma once
#include "Object.h"

class Geometry;
class Camera;

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
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;

public:
    XMFLOAT3    GetPosition();
    void        SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

protected:
    ID3D12Device*                       m_pDevice;
    ID3D12GraphicsCommandList*          m_pCmdLst;
    unique_ptr<UploadBuffer<ObjectCB>>	m_ObjectCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_DiffuseCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_AmbientCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_SpecularCB;

    Camera*                             m_pCamera;
};

