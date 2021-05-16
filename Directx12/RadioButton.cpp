#include "RadioButton.h"
#include "KeyMgr.h"

RadioButton::RadioButton(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName)
    : Button(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName, strMouseOnTextureName, strActiveTextureName)
{
}

RadioButton::~RadioButton()
{
}

int RadioButton::Update(const float& fTimeDelta)
{
    UI::Update(fTimeDelta);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    switch (m_eButtonState)
    {
    case NONE:
        if (PtInRect(&m_Rect, pt))
        {
            SetButtonState(BUTTON_STATE::MOUSEON);
            if (KeyMgr::GetInstance()->KeyDown(VK_LBUTTON))
                SetButtonState(BUTTON_STATE::ON);
        }
    case MOUSEON:
        if (PtInRect(&m_Rect, pt))
        {
            SetButtonState(BUTTON_STATE::MOUSEON);
            if (KeyMgr::GetInstance()->KeyDown(VK_LBUTTON))
                SetButtonState(BUTTON_STATE::ON);
        }
        else
            SetButtonState(BUTTON_STATE::NONE);
        break;
    case ON:
        if (PtInRect(&m_Rect, pt))
        {
            if (KeyMgr::GetInstance()->KeyDown(VK_LBUTTON))
                SetButtonState(BUTTON_STATE::NONE);
        }
        break;
    default:
        break;
    }
    
   
    return 0;
}

void RadioButton::LateUpdate(const float& fTimeDelta)
{
    Button::LateUpdate(fTimeDelta);
}

void RadioButton::Render(const float& fTimeDelta, int _instanceCount)
{
    Button::Render(fTimeDelta);
}

void RadioButton::SetButtonState(BUTTON_STATE eState)
{
    if (m_eButtonState == eState)
        return;

    m_eButtonState = eState;

    if (m_eButtonState == BUTTON_STATE::ON)
        for (auto button : m_lstOthers)
            button->SetButtonState(BUTTON_STATE::NONE);

}