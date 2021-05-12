#pragma once
#include "UI.h"
class Panel :
    public UI
{
public:
    Panel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName);
    virtual ~Panel();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
private:
    float    m_fScale;
};

