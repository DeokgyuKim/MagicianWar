#include "ShopController.h"
#include "Scene.h"
#include "SkillController.h"
#include "TextController.h"
#include "Cursor.h"

void buyskillQ();
void buyskillE();

ShopController* ShopController::m_pInstance = NULL;
ShopController::ShopController()
{
}

ShopController::~ShopController()
{
}

void ShopController::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene, int CharType)
{
	m_pScene = pScene;

	m_pCursor = new Cursor(device, cmdLst, pRenderer);

	XMFLOAT4 pos;
	pos.x = WINCX / 2.f - 500.f;
	pos.y = WINCY / 2.f - 128.f;
	pos.z = 320.f;
	pos.w = 352.f;

	m_pSkillBase[0] = new UI(device, cmdLst, pRenderer, pos, "ShopBase");

	for (int i = 0; i < 4; ++i)
	{
		XMFLOAT4 onpos;
		onpos.x = pos.x + 256.f;
		onpos.y = pos.y + 10.f + 60.f * i;
		onpos.z = 54.f;
		onpos.w = 50.f;

		m_pSkillOn[0][i] = new UI(device, cmdLst, pRenderer, onpos, "SkillOn");
	}
	XMFLOAT4 buttonpos;
	buttonpos.x = pos.x + 10.f;
	buttonpos.y = pos.y + 10.f;
	buttonpos.z = 236.f;
	buttonpos.w = 236.f;

	if (CharType == WIZARD_FIRE)
	{
		m_pSkillIcon[0] = new UI(device, cmdLst, pRenderer, buttonpos, "SkillFire1");
		m_iCost[0] = 50;
		m_iCost[1] = 100;
	}
	else if (CharType == WIZARD_COLD)
	{
		m_iCost[0] = 150;
		m_iCost[1] = 50;
		m_pSkillIcon[0] = new UI(device, cmdLst, pRenderer, buttonpos, "SkillCold1");
	}
	else if (CharType == WIZARD_DARKNESS)
	{
		m_iCost[0] = 100;
		m_iCost[1] = 50;
		m_pSkillIcon[0] = new UI(device, cmdLst, pRenderer, buttonpos, "SkillDarkness1");
	}

	m_pButton[0] = new ClickerButton(device, cmdLst, pRenderer, buttonpos, "ShopNone", "ShopMouse", "ShopOn");
	m_pButton[0]->SetEventButtonOn(buyskillQ);

	buttonpos.x = WINCX / 2.f - 480.f;
	buttonpos.y = pos.y + 246.f;
	buttonpos.z = 100.f;
	buttonpos.w = 100.f;
	m_pCostTxtCtrl[0] = new TextController(device, cmdLst, pRenderer, buttonpos, to_string(m_iCost[0]).c_str(), pScene);

	pos.x = WINCX / 2.f + 180.f;

	m_pSkillBase[1] = new UI(device, cmdLst, pRenderer, pos, "ShopBase");

	for (int i = 0; i < 4; ++i)
	{
		XMFLOAT4 onpos;
		onpos.x = pos.x + 256.f;
		onpos.y = pos.y + 10.f + 60.f * i;
		onpos.z = 54.f;
		onpos.w = 50.f;

		m_pSkillOn[1][i] = new UI(device, cmdLst, pRenderer, onpos, "SkillOn");
	}
	buttonpos.x = pos.x + 10.f;
	buttonpos.y = pos.y + 10.f;
	buttonpos.z = 236.f;
	buttonpos.w = 236.f;

	if (CharType == WIZARD_FIRE)
	{
		m_pSkillIcon[1] = new UI(device, cmdLst, pRenderer, buttonpos, "SkillFire2");
	}
	else if (CharType == WIZARD_COLD)
	{
		m_pSkillIcon[1] = new UI(device, cmdLst, pRenderer, buttonpos, "SkillCold2");
	}
	else if (CharType == WIZARD_DARKNESS)
	{
		m_pSkillIcon[1] = new UI(device, cmdLst, pRenderer, buttonpos, "SkillDarkness2");
	}

	m_pButton[1] = new ClickerButton(device, cmdLst, pRenderer, buttonpos, "ShopNone", "ShopMouse", "ShopOn");
	m_pButton[1]->SetEventButtonOn(buyskillE);

	buttonpos.x = WINCX / 2.f + 200.f;
	buttonpos.y = pos.y + 246.f;
	buttonpos.z = 100.f;
	buttonpos.w = 100.f;
	m_pCostTxtCtrl[1] = new TextController(device, cmdLst, pRenderer, buttonpos, to_string(m_iCost[1]).c_str(), pScene);

	m_pGoldTxtCtrl = new TextController(device, cmdLst, pRenderer, XMFLOAT4(280.f, 67.f, 60.f, 60.f), to_string(m_iGold).c_str(), pScene);
}

void ShopController::Update()
{
	int cnt = m_pSkillCtrl->GetSkillCnt(0);
	for (int i = 0; i < cnt; ++i)
		m_pSkillOn[0][i]->SetTextureName("SkillOn");
	for (int i = cnt; i < 4; ++i)
		m_pSkillOn[0][i]->SetTextureName("Ui_Text_No");

	cnt = m_pSkillCtrl->GetSkillCnt(1);
	for (int i = 0; i < cnt; ++i)
		m_pSkillOn[1][i]->SetTextureName("SkillOn");
	for (int i = cnt; i < 4; ++i)
		m_pSkillOn[1][i]->SetTextureName("Ui_Text_No");
}

void ShopController::SetRendering(bool On)
{
	if (On)
	{
		if (!m_bShopTime)
		{
			m_bShopTime = true;
			for (int i = 0; i < 2; ++i)
			{
				m_pScene->PushObject(m_pSkillBase[i], OBJ_TYPE::OBJ_UI);
				for (int j = 0; j < 4; ++j)
				{
					m_pScene->PushObject(m_pSkillOn[i][j], OBJ_TYPE::OBJ_UI);
				}
				m_pScene->PushObject(m_pSkillIcon[i], OBJ_TYPE::OBJ_UI);

				XMFLOAT4 buttonpos;
				buttonpos.x = WINCX / 2.f - 480.f;
				buttonpos.y = WINCY / 2.f - 128.f + 246.f;
				buttonpos.z = 100.f;
				buttonpos.w = 100.f;
				m_pCostTxtCtrl[0]->Initialize(buttonpos, to_string(m_iCost[0]).c_str(), m_pScene);

				buttonpos.x = WINCX / 2.f + 200.f;
				buttonpos.y = WINCY / 2.f - 128.f + 246.f;
				buttonpos.z = 100.f;
				buttonpos.w = 100.f;
				m_pCostTxtCtrl[1]->Initialize(buttonpos, to_string(m_iCost[1]).c_str(), m_pScene);

				m_pScene->PushObject(m_pButton[i], OBJ_TYPE::OBJ_UI);
				m_pScene->PushObject(m_pCursor, OBJ_TYPE::OBJ_UI);
			}
		}

	}
	else
	{
		if (m_bShopTime)
		{
			m_bShopTime = false;
			for (int i = 0; i < 2; ++i)
			{
				m_pScene->RemoveObject(m_pSkillBase[i], OBJ_TYPE::OBJ_UI);
				for (int j = 0; j < 4; ++j)
				{
					m_pScene->RemoveObject(m_pSkillOn[i][j], OBJ_TYPE::OBJ_UI);
				}
				m_pScene->RemoveObject(m_pSkillIcon[i], OBJ_TYPE::OBJ_UI);

				m_pCostTxtCtrl[0]->RemoveTexts(m_pScene);
				m_pCostTxtCtrl[1]->RemoveTexts(m_pScene);

				m_pScene->RemoveObject(m_pButton[i], OBJ_TYPE::OBJ_UI);
				m_pScene->RemoveObject(m_pCursor, OBJ_TYPE::OBJ_UI);
			}
		}
	}
}

bool ShopController::AddCoin(int coin)
{
	if (m_iGold + coin < 0)
		return false;

	m_iGold += coin;

	m_pGoldTxtCtrl->RemoveTexts(m_pScene);
	string gold = to_string(m_iGold);
	m_pGoldTxtCtrl->Initialize(XMFLOAT4(280.f, 67.f, 60.f, 60.f), gold.c_str(), m_pScene);

	return true;
}

void ShopController::Release()
{
	for (int i = 0; i < 2; ++i)
	{
		delete m_pSkillBase[i];
		for (int j = 0; j < 4; ++j)
			delete m_pSkillOn[i][j];
		delete m_pButton[i];

	}
	delete m_pGoldTxtCtrl;
}

void buyskillQ()
{
	int cnt = SkillController::GetInstance()->GetSkillCnt(0);
	if (cnt >= 4)
		return;
	if (!ShopController::GetInstance()->AddCoin(-ShopController::GetInstance()->GetCost(0)))
		return;
	SkillController::GetInstance()->SetSkillCnt(0, cnt + 1);
}
void buyskillE()
{
	int cnt = SkillController::GetInstance()->GetSkillCnt(1);
	if (cnt >= 4)
		return;
	if (!ShopController::GetInstance()->AddCoin(-ShopController::GetInstance()->GetCost(1)))
		return;
	SkillController::GetInstance()->SetSkillCnt(1, cnt + 1);
}