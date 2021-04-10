#pragma once
#include "Object.h"

class Geometry;
class Camera;

class Bullet :
    public Object
{
public:
    Bullet(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string _meshName,
        XMFLOAT3 Pos, XMFLOAT3 Rotate, float speed);
    ~Bullet();
private:
    void    Initialize(XMFLOAT3 Pos, XMFLOAT3 Rotate);
    void    Release();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
public:

public:
    XMFLOAT3    GetPosition();

private:
    void UpdateSkinnedAnimation(const float fTimeDelta);

protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB;
    
    float       m_fSpeed;
};

