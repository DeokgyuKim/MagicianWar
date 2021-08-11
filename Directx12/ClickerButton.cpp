#include "ClickerButton.h"
#include "KeyMgr.h"

ClickerButton::ClickerButton(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName)
	: Button(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName, strMouseOnTextureName, strActiveTextureName)
{
}

ClickerButton::~ClickerButton()
{
}

int ClickerButton::Update(const float& fTimeDelta)
{
	UI::Update(fTimeDelta);
	if (m_fMouseTime > 0)
	{
		m_fMouseTime -= fTimeDelta;
		if (m_fMouseTime <= 0.f)
		{
			m_fMouseTime = 0.f;
			m_eButtonState = BUTTON_STATE::NONE;
		}
		return 0;
	}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	switch (m_eButtonState)
	{
	case NONE:
		if (PtInRect(&m_Rect, pt))
		{
			m_eButtonState = BUTTON_STATE::MOUSEON;
		}
	case MOUSEON:
		if (PtInRect(&m_Rect, pt))
		{
			m_eButtonState = BUTTON_STATE::MOUSEON;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				m_eButtonState = BUTTON_STATE::ON;
				cout << "Button click!" << endl;
				m_fMouseTime = 0.1f;
				if (ButtonOnfp != nullptr)
					ButtonOnfp();
			}
		}
		else
			m_eButtonState = BUTTON_STATE::NONE;
		break;
	default:
		break;
	}
	return 0;
}
