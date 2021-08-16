#pragma once
#include "ClickerButton.h"

class Cursor;
class Scene;
class SkillController;
class TextController;
class ShopController
{
private:
	ShopController();
	~ShopController();
public:
	static ShopController* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new ShopController();
		return m_pInstance;
	}
	static void DestroyInstance() {
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static ShopController* m_pInstance;
public:
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene, int CharType);
	void Update();
	void SetSkillCtrl(SkillController* pCtrl) { m_pSkillCtrl = pCtrl; }
	void SetRendering(bool On);
	bool AddCoin(int coin);
private:
	void Release();
protected:
	UI*					m_pSkillBase[2];
	UI*					m_pSkillIcon[2];
	UI*					m_pSkillOn[2][4];
	ClickerButton*		m_pButton[2];
	Cursor*				m_pCursor;

	int					m_iGold = 0;
	TextController*		m_pGoldTxtCtrl;

	SkillController*	m_pSkillCtrl;
	Scene*				m_pScene;
	bool				m_bShopTime = false;
};

