#include "SkillController.h"
#include "Scene.h"

SkillController* SkillController::m_pInstance = NULL;
SkillController::SkillController()
{
}

SkillController::~SkillController()
{
}
void SkillController::SetSkillCnt(int idx, int cnt)
{ 
	if (idx < 0 || idx > 1)
		return;

	m_iSkillCnt[idx] = min(max(cnt, 0), 4);

	for (int i = 0; i < cnt; ++i)
		m_pSkillOn[idx][i]->SetTextureName("SkillOn");
	for (int i = cnt; i < 4; ++i)
		m_pSkillOn[idx][i]->SetTextureName("Ui_Text_No");
}
void SkillController::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene)
{
	XMFLOAT4 pos;
	pos.x = WINCX / 2.f - 300.f;
	pos.y = WINCY - 200.f;
	pos.z = 160.f;
	pos.w = 128.f;

	m_pSkillBase[0] = new UI(device, cmdLst, pRenderer, pos, "SkillBase");
	pScene->PushObject(m_pSkillBase[0], OBJ_TYPE::OBJ_UI);

	for (int i = 0; i < 4; ++i)
	{
		XMFLOAT4 onpos;
		onpos.x = pos.x + 128.f;
		onpos.y = pos.y + 5.f + 30.f * i;
		onpos.z = 27.f;
		onpos.w = 25.f;
		
		m_pSkillOn[0][i] = new UI(device, cmdLst, pRenderer, onpos, "Ui_Text_No");
		pScene->PushObject(m_pSkillOn[0][i], OBJ_TYPE::OBJ_UI);
	}

	pos.x = WINCX / 2.f + 140.f;

	m_pSkillBase[1] = new UI(device, cmdLst, pRenderer, pos, "SkillBase");
	pScene->PushObject(m_pSkillBase[1], OBJ_TYPE::OBJ_UI);

	for (int i = 0; i < 4; ++i)
	{
		XMFLOAT4 onpos;
		onpos.x = pos.x + 128.f;
		onpos.y = pos.y + 5.f + 30.f * i;
		onpos.z = 27.f;
		onpos.w = 25.f;

		m_pSkillOn[1][i] = new UI(device, cmdLst, pRenderer, onpos, "Ui_Text_No");
		pScene->PushObject(m_pSkillOn[1][i], OBJ_TYPE::OBJ_UI);
	}
}

