#include "Button.h"
#include "KeyMgr.h"

Button::Button(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName)
    : UI(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName)
{
    m_strActiveTextureName = strActiveTextureName;
    m_strMouseOnTextureName = strMouseOnTextureName;
    m_Rect.left = xmfInfo.x;
    m_Rect.bottom = xmfInfo.y + xmfInfo.w;
    m_Rect.right = xmfInfo.x + xmfInfo.z;
    m_Rect.top = xmfInfo.y;
    m_eButtonState = BUTTON_STATE::NONE;
}

Button::~Button()
{
}

int Button::Update(const float& fTimeDelta)
{
    UI::Update(fTimeDelta);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    cout << "x : " << pt.x << ", y : " << pt.y << endl;

    switch (m_eButtonState)
    {
    case NONE:
        if (PtInRect(&m_Rect, pt))
        {
            m_eButtonState = BUTTON_STATE::MOUSEON;
            if (KeyMgr::GetInstance()->KeyDown(VK_LBUTTON))
                m_eButtonState = BUTTON_STATE::ON;
        }
    case MOUSEON:
        if (PtInRect(&m_Rect, pt))
        {
            m_eButtonState = BUTTON_STATE::MOUSEON;
            if (KeyMgr::GetInstance()->KeyDown(VK_LBUTTON))
                m_eButtonState = BUTTON_STATE::ON;
        }
        else
            m_eButtonState = BUTTON_STATE::NONE;
        break;
    case ON:
        if (PtInRect(&m_Rect, pt))
        {
            if (KeyMgr::GetInstance()->KeyDown(VK_LBUTTON))
                m_eButtonState = BUTTON_STATE::NONE;
        }
        break;
    default:
        break;
    }
    
   
    return 0;
}

void Button::LateUpdate(const float& fTimeDelta)
{
    UI::LateUpdate(fTimeDelta);
}

void Button::Render(const float& fTimeDelta)
{
    UI::Render(fTimeDelta);
}

string Button::GetTextureName()
{
    switch (m_eButtonState)
    {
    case NONE:
        return m_strTextureName;
        break;
    case MOUSEON:
        if (m_strMouseOnTextureName != "")
            return m_strMouseOnTextureName;
        return m_strTextureName;
        break;
    case ON:
        if (m_strActiveTextureName != "")
            return m_strActiveTextureName;
        return m_strTextureName;
        break;
        default:
        break;
    }
    return "";
}
