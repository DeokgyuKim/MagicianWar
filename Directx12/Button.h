#pragma once
#include "UI.h"
class Button :
    public UI
{
public:
    Button(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
        XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName);
    virtual ~Button();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
    virtual string	GetTextureName() override;
    BUTTON_STATE GetButtonState() { return m_eButtonState; }
private:
    string          m_strMouseOnTextureName;
    string          m_strActiveTextureName;
    RECT            m_Rect;
    BUTTON_STATE    m_eButtonState;
};

