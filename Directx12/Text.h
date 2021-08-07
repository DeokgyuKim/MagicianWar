#pragma once
#include "UI.h"
class Text :
    public UI
{
public:
    Text(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, XMFLOAT2 Idx);
    virtual ~Text();
public:
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
public:
    void SetScrollPos(float fRatio) { m_xmfRatio.y = fRatio; }
    void SetTextColor(XMFLOAT3 fColor) { m_xmfColor = fColor; }
protected:
    unique_ptr<UploadBuffer<TextCB>>    m_TextCB;
    XMFLOAT2                            m_xmfScorllPos;
    XMFLOAT3                            m_xmfColor;
};

