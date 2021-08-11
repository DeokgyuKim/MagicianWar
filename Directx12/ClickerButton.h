#pragma once
#include "Button.h"
class ClickerButton :
    public Button
{
public:
    ClickerButton(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName);
    virtual ~ClickerButton();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
protected:
    float m_fMouseTime = 0.f;
};

