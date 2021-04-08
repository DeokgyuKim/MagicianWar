#pragma once
#include "Object.h"
class StaticObject :
    public Object
{
public:
    StaticObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName);
    ~StaticObject();
private:
    void    Initialize(XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName);
    void    Release();
    HRESULT BuildConstantBuffer();
public:
    void UpdateObjectCB();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;

protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB;
};

