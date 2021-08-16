#include "Meteor.h"

#include "Meteor_Body.h"
#include "Meteor_FireEff.h"
#include "Meteor_FireEff2.h"
#include "Transform.h"

#include "MainApp.h"
#include "Scene.h"
#include "Boom.h"

Meteor::Meteor(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetRotate(rotate);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
	m_eSkillType = SKILL_TYPE::SKILL_FIRE2;
}

Meteor::~Meteor()
{
	XMFLOAT3 pos = dynamic_cast<Transform*>(GetTransController())->GetPosition();
	MainApp::GetInstance()->GetScene()->PushObject(new Boom(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(5.f, 5.f, 5.f), pos, "FireBall2"), OBJ_TYPE::OBJ_SKILL);
}

void Meteor::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void Meteor::BuildSkillEffects()
{
	SkillEff* pEff = new Meteor_Body(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
	pEff = new Meteor_FireEff(m_pDevice, m_pCmdLst, m_pRenderer, this, XMFLOAT3(0.f, 0.f, 0.f), 1);
	m_vecSkillEff.push_back(pEff);
	pEff = new Meteor_FireEff(m_pDevice, m_pCmdLst, m_pRenderer, this, XMFLOAT3(0.f, 0.f, 45.f), 1);
	m_vecSkillEff.push_back(pEff);
	//pEff = new Meteor_FireEff2(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//m_vecSkillEff.push_back(pEff);
}

int Meteor::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
	return 0;
}

void Meteor::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void Meteor::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
