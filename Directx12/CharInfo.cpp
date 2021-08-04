#include "CharInfo.h"
#include "Scene.h"

CharInfo::CharInfo(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strTextureName)
	: UI(device, cmdLst, pRenderer, xmfInfo, strTextureName)
{
	XMFLOAT4 xmfPortrait;
	xmfPortrait.x = xmfInfo.x + 20.f;
	xmfPortrait.y = xmfInfo.y + 20.f;
	xmfPortrait.z = 100.f;
	xmfPortrait.w = 100.f;

	m_pPortrait = new UI(device, cmdLst, pRenderer, xmfPortrait, "No");
}

CharInfo::~CharInfo()
{
}

int CharInfo::Update(const float& fTimeDelta)
{
	UI::Update(fTimeDelta);

	if (m_pRoomPlayer != nullptr)
	{
		if (m_pRoomPlayer->host == true)
			m_strTextTextureName = "Ui_Text_CharHost";
		else if (m_pRoomPlayer->readystate == true)
			m_strTextTextureName = "Ui_Text_CharReady";
		else
			m_strTextTextureName = "";

		if (m_pRoomPlayer->chartype == WIZARD_FIRE)
			m_pPortrait->SetTextureName("Ui_Char_Fire");
		else if (m_pRoomPlayer->chartype == WIZARD_COLD)
			m_pPortrait->SetTextureName("Ui_Char_Cold");
		else if (m_pRoomPlayer->chartype == WIZARD_DARKNESS)
			m_pPortrait->SetTextureName("Ui_Char_Darkness");
		else
			m_pPortrait->SetTextureName("Ui_Text_No");


		if (m_pRoomPlayer->used == false)
		{
			m_strTextTextureName = "";
			m_pPortrait->SetTextureName("Ui_Text_No");
		}
	}
	else
	{
		m_strTextTextureName = "";
		m_pPortrait->SetTextureName("Ui_Text_No");
	}

	return 0;
}

void CharInfo::SetRoomPlayer(Ui_Roomplayer* pPlayer)
{
	if (pPlayer != nullptr)
		m_pRoomPlayer = pPlayer;
}

void CharInfo::PushPortraitInScene(Scene* pScene)
{
	pScene->PushObject(m_pPortrait, OBJ_TYPE::OBJ_UI);
}
