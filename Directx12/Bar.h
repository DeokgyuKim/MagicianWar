#pragma once
#include "UI.h"
class Bar :
    public UI
{
public:
    Bar(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
        XMFLOAT4 xmfInfo, string strBaseTextureName);
    virtual ~Bar();
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
};

