#pragma once
#include "Object.h"
class Weapon :
    public Object
{
public:
    Weapon(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string meshName, string textureName,
        Object* pParent, int socketIdx, XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPos);
    ~Weapon();
private:
    void    Initialize(string MeshName, string textureName, XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPos);
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

    int         m_iSocketIdx;
    XMFLOAT4X4  m_xmf4x4ParentWorld;
    Object*     m_pParent
};

