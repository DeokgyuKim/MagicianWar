#include "Button.h"
#include "KeyMgr.h"
#include "Network.h"

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
	m_prevButtonState = BUTTON_STATE::NONE;
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

	if (m_prevButtonState == BUTTON_STATE::NONE && m_eButtonState == BUTTON_STATE::ON)
	{	// 버튼 활성화
		m_prevButtonState = BUTTON_STATE::ON;
		if (m_iTag == BUTTON_ROOM_MAKE)
		{   // Room 만들기 키
			Network::GetInstance()->CallEvent(EVENT_LOBBY_ROOM_MAKE_REQUEST,0);
		}
		else if (m_iTag == BUTTON_ROOM_JOIN)
		{ // Room 들어가기 키
			Network::GetInstance()->CallEvent(EVENT_LOBBY_ROOM_JOIN_REQUEST,0);
		}
		else if (m_iTag == BUTTON_GAME_READY)
		{ // 게임 시작 키
			Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_READY_ON, 0);
		}
		else if (m_iTag == BUTTON_ROOM_EXIT)
		{ // 방 나가기
			Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_EXIT, 0);
		}
	}
	else if (m_prevButtonState == BUTTON_STATE::ON && m_eButtonState == BUTTON_STATE::NONE)
	{	// 버튼 비활성화
		m_prevButtonState = BUTTON_STATE::NONE;
		if (m_iTag == BUTTON_ROOM_MAKE)
		{   // Room 만들기 키

		}
		else if (m_iTag == BUTTON_ROOM_JOIN)
		{ // Room 들어가기 키

		}
		else if (m_iTag == BUTTON_GAME_READY)
		{ // 게임 레디 키
			Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_READY_OFF, 0);
		}
	}



	return 0;
}

void Button::LateUpdate(const float& fTimeDelta)
{
	UI::LateUpdate(fTimeDelta);
}

void Button::Render(const float& fTimeDelta, int _instanceCount)
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
