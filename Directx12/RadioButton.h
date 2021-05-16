#pragma once
#include "Button.h"

class RadioButton :
    public Button
{
public:
    RadioButton(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
        XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName);
    virtual ~RadioButton();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
private:
    void SetButtonState(BUTTON_STATE eState);
public:
    void SetOthers(RadioButton* pRadio) { m_lstOthers.push_back(pRadio); }
protected:
    list<RadioButton*> m_lstOthers;
};

