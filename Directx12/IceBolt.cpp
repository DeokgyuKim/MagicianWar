#include "IceBolt.h"

#include "IceBolt_Ice.h"
#include "IceBolt_Body.h"
#include "IceBolt_Tornado.h"
#include "Transform.h"

IceBolt::IceBolt(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetRotate(rotate);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
}

IceBolt::~IceBolt()
{
}

void IceBolt::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void IceBolt::BuildSkillEffects()
{
	//SkillEff* pEff = new IceBolt_Ice(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//m_vecSkillEff.push_back(pEff);
	SkillEff* pEff = new IceBolt_Body(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
	pEff = new IceBolt_Tornado(m_pDevice, m_pCmdLst, m_pRenderer,XMFLOAT3(180.f,0.f,0.f), this);
	m_vecSkillEff.push_back(pEff);
	pEff = new IceBolt_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 70.f, 0.f), this);
	m_vecSkillEff.push_back(pEff);
	pEff = new IceBolt_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 140.f, 0.f), this);
	m_vecSkillEff.push_back(pEff);
	pEff = new IceBolt_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 210.f, 0.f), this);
	m_vecSkillEff.push_back(pEff);
	pEff = new IceBolt_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 280.f, 0.f), this);
	m_vecSkillEff.push_back(pEff);

}

int IceBolt::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
	return 0;
}

void IceBolt::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void IceBolt::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
