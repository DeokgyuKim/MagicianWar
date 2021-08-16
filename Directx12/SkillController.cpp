#include "SkillController.h"
#include "Scene.h"
#include "NetSkill.h"
#include "Camera.h"

SkillController* SkillController::m_pInstance = NULL;
SkillController::SkillController()
{
}

SkillController::~SkillController()
{
	Release();
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
bool SkillController::UseSkill(int idx)
{
	if (idx < 0 || idx > 1)
		return false;
	if (m_fSkillCurCool[idx] > 0.f)
		return false;
	if (m_iSkillCnt[idx] <= 0)
		return false;
	if (m_fSkillLeft > 0)
		return false;

	m_fSkillCurCool[idx] = m_fSkillCoolTime[idx];
	m_fSkillLeft = 1.4f;
	SetSkillCnt(idx, m_iSkillCnt[idx] - 1);

	return true;
}
void SkillController::UseSkillFailed(int idx)
{
	m_fSkillCurCool[idx] = 0.f;
	SetSkillCnt(idx, m_iSkillCnt[idx] + 1);
}
XMFLOAT3 SkillController::GeneratePositionForPacket(int idx)
{
	return m_pNetSkill[idx]->GetPosition();
}
XMFLOAT3 SkillController::GenerateRotateForPacket(int idx)
{
	return m_pNetSkill[idx]->GetRotate();
}
void SkillController::Release()
{
	//for (int i = 0; i < 2; ++i)
	//{
	//	delete m_pSkillBase[i];
	//	for (int j = 0; j < 4; ++j)
	//		delete m_pSkillOn[i][j];
	//	delete m_pNetSkill[i];
	//}
}
void SkillController::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene, int CharType)
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

	if (CharType == WIZARD_FIRE)
	{
		m_pNetSkill[0] = new NetSkill(pScene->GetPlayer(), dynamic_cast<Camera*>(pScene->GetCamera()), SKILL_GENTYPE::SKILL_FLAMEWALL);
		m_pNetSkill[1] = new NetSkill(pScene->GetPlayer(), dynamic_cast<Camera*>(pScene->GetCamera()), SKILL_GENTYPE::SKILL_METEOR);

		pos.x = WINCX / 2.f - 295.f;
		pos.y = WINCY - 195.f;
		pos.z = 118.f;
		pos.w = 118.f;
		m_pSkillIcon[0] = new UI(device, cmdLst, pRenderer, pos, "SkillFire1");
		pScene->PushObject(m_pSkillIcon[0], OBJ_TYPE::OBJ_UI);

		pos.x = WINCX / 2.f + 145.f;
		m_pSkillIcon[1] = new UI(device, cmdLst, pRenderer, pos, "SkillFire2");
		pScene->PushObject(m_pSkillIcon[1], OBJ_TYPE::OBJ_UI);

		SetSkillCoolTime(0, 10.f);
		SetSkillCoolTime(1, 7.f);
	}
	else if (CharType == WIZARD_COLD)
	{
		m_pNetSkill[0] = new NetSkill(pScene->GetPlayer(), dynamic_cast<Camera*>(pScene->GetCamera()), SKILL_GENTYPE::SKILL_ICEFIELD);
		m_pNetSkill[1] = new NetSkill(pScene->GetPlayer(), dynamic_cast<Camera*>(pScene->GetCamera()), SKILL_GENTYPE::SKILL_FREEZE);

		pos.x = WINCX / 2.f - 295.f;
		pos.y = WINCY - 195.f;
		pos.z = 118.f;
		pos.w = 118.f;
		m_pSkillIcon[0] = new UI(device, cmdLst, pRenderer, pos, "SkillCold1");
		pScene->PushObject(m_pSkillIcon[0], OBJ_TYPE::OBJ_UI);

		pos.x = WINCX / 2.f + 145.f;
		m_pSkillIcon[1] = new UI(device, cmdLst, pRenderer, pos, "SkillCold2");
		pScene->PushObject(m_pSkillIcon[1], OBJ_TYPE::OBJ_UI);

		SetSkillCoolTime(0, 15.f);
		SetSkillCoolTime(1, 10.f);
	}
	else if (CharType == WIZARD_DARKNESS)
	{
		m_pNetSkill[0] = new NetSkill(pScene->GetPlayer(), dynamic_cast<Camera*>(pScene->GetCamera()), SKILL_GENTYPE::SKILL_ENCHAN);
		m_pNetSkill[1] = new NetSkill(pScene->GetPlayer(), dynamic_cast<Camera*>(pScene->GetCamera()), SKILL_GENTYPE::SKILL_HATRED);

		pos.x = WINCX / 2.f - 295.f;
		pos.y = WINCY - 195.f;
		pos.z = 118.f;
		pos.w = 118.f;
		m_pSkillIcon[0] = new UI(device, cmdLst, pRenderer, pos, "SkillDarkness1");
		pScene->PushObject(m_pSkillIcon[0], OBJ_TYPE::OBJ_UI);

		pos.x = WINCX / 2.f + 145.f;
		m_pSkillIcon[1] = new UI(device, cmdLst, pRenderer, pos, "SkillDarkness2");
		pScene->PushObject(m_pSkillIcon[1], OBJ_TYPE::OBJ_UI);

		SetSkillCoolTime(0, 12.f);
		SetSkillCoolTime(1, 7.f);
	}
}

void SkillController::Update(const float& fTimeDelta)
{
	m_fSkillCurCool[0] = max(m_fSkillCurCool[0] - fTimeDelta, 0.f);
	m_fSkillCurCool[1] = max(m_fSkillCurCool[1] - fTimeDelta, 0.f);
	m_fSkillLeft = max(m_fSkillLeft - fTimeDelta, 0.f);
}

