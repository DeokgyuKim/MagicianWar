#pragma once
#include "Object.h"
class StaticObject :
    public Object
{
public:
    StaticObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName, bool bPhysx = false);
    ~StaticObject();
private:
    void    Initialize(XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName, bool bPhysx);
    void    Release();
public:

public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    pxMeshVtxInfo   m_tPxMeshVtxInfo;
};

