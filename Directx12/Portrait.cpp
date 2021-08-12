#include "Portrait.h"
#include "Renderer.h"
#include "Player.h"

Portrait::Portrait(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, Player* pPlayer)
	: UI(device, cmdLst, pRenderer, xmfInfo, "")
{
	m_pPlayer = pPlayer;

	switch (m_pPlayer->GetNetworkInfo().CharacterType)
	{
	case WIZARD_FIRE:
		m_strTextureName = "Ui_Char_Fire";
		break;
	case WIZARD_COLD:
		m_strTextureName = "Ui_Char_Cold";
		break;
	case WIZARD_DARKNESS:
		m_strTextureName = "Ui_Char_Darkness";
		break;
	}
	m_xmfRatio.x = 1.f;
}

Portrait::~Portrait()
{
}

int Portrait::Update(const float& fTimeDelta)
{
	if (m_pPlayer->GetNetworkInfo().iHp <= 0)
		m_xmfRatio.x = 0.5f;
	else
		m_xmfRatio.x = 1.f;
	return 0;
}

void Portrait::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);
	m_pRenderer->PushObject(RENDER_TYPE::RENDER_UI, this);
	UiCB uicb;
	uicb.ratio = m_xmfRatio;
	m_UiCB->CopyData(0, uicb);
}
