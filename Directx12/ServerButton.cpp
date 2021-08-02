#include "ServerButton.h"
#include "Button.h"
#include "Com_Network.h"
#include "Network.h"

ServerButton::ServerButton(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName)
    :Button(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName, strMouseOnTextureName, strActiveTextureName)
{
	
}

ServerButton::~ServerButton()
{
}

int ServerButton::Update(const float& fTimeDelta)
{
    Button::Update(fTimeDelta);

	if (m_prevButtonState == BUTTON_STATE::NONE && m_eButtonState == BUTTON_STATE::ON)
	{	// ��ư Ȱ��ȭ
		m_prevButtonState = BUTTON_STATE::ON;
		if (m_iTag == BUTTON_ROOM_MAKE)
		{   // Room ����� Ű
			dynamic_cast<Com_Network*>(m_mapComponent["Com_Network"])->CallEvent(EVENT_LOBBY_ROOM_MAKE_REQUEST);
		}
		else if (m_iTag == BUTTON_ROOM_JOIN)
		{ // Room ���� Ű
			dynamic_cast<Com_Network*>(m_mapComponent["Com_Network"])->CallEvent(EVENT_LOBBY_ROOM_JOIN_REQUEST);
		}
		else if (m_iTag == BUTTON_GAME_READY)
		{ // ���� ���� Ű
			dynamic_cast<Com_Network*>(m_mapComponent["Com_Network"])->CallEvent(EVENT_ROOM_PLAYER_READY_ON);
		}
		else if (m_iTag == BUTTON_ROOM_EXIT)
		{ // �� ������
			dynamic_cast<Com_Network*>(m_mapComponent["Com_Network"])->CallEvent(EVENT_ROOM_PLAYER_EXIT);
		}
	}
	else if (m_prevButtonState == BUTTON_STATE::ON && m_eButtonState == BUTTON_STATE::NONE)
	{	// ��ư ��Ȱ��ȭ
		m_prevButtonState = BUTTON_STATE::NONE;
		if (m_iTag == BUTTON_ROOM_MAKE)
		{   // Room ����� Ű

		}
		else if (m_iTag == BUTTON_ROOM_JOIN)
		{ // Room ���� Ű

		}
		else if (m_iTag == BUTTON_GAME_READY)
		{ // ���� ���� Ű
			dynamic_cast<Com_Network*>(m_mapComponent["Com_Network"])->CallEvent(EVENT_ROOM_PLAYER_READY_OFF);
		}
	}

    return 0;
}

void ServerButton::LateUpdate(const float& fTimeDelta)
{
    Button::LateUpdate(fTimeDelta);
}

void ServerButton::Render(const float& fTimeDelta, int _instanceCount)
{
    Button::Render(fTimeDelta);
}

string ServerButton::GetTextureName()
{
    string name = Button::GetTextureName();
    
    return name;
}

void ServerButton::SetTag(const int& iTag, const int& _event)
{
	m_iTag = iTag;
	Component* pComponent = new Com_Network(iTag, _event);
	m_mapComponent["Com_Network"] = pComponent;
}
