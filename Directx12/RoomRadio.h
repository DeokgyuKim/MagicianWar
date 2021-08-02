#pragma once
#include "RadioButton.h"
class RoomRadio :
    public RadioButton
{
public:
    RoomRadio(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName);
    virtual ~RoomRadio();
public:
    virtual void LateUpdate(const float& fTimeDelta) override;
public:
    void SetScrollPos(float Scorll, float fRatio);
protected:
    XMFLOAT4 m_xmfInfo;
};

