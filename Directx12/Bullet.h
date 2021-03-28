#pragma once
#include "Object.h"

class Geometry;
class Camera;

class Bullet :
    public Object
{
public:
    Bullet(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
        XMFLOAT3 Pos, XMFLOAT3 Rotate, float speed);
    ~Bullet();
private:
    void    Initialize(XMFLOAT3 Pos, XMFLOAT3 Rotate);
    void    Release();
    HRESULT BuildConstantBuffer();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;

public:
    XMFLOAT3    GetPosition();

private:
    void UpdateSkinnedAnimation(const float fTimeDelta);

protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB;
    unique_ptr<UploadBuffer<MaterialCB>>        m_MaterialCB;
    
    float       m_fSpeed;
};

