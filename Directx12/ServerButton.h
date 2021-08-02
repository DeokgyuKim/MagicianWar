#pragma once
#include "Button.h"

class ServerButton :
    public Button
{
public:
    ServerButton(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName);
    virtual ~ServerButton();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
    virtual string	GetTextureName() override;

public:
    void SetTag(const int& iTag, const int& _event);
protected:

};

