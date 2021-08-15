#include "HatredChain.h"

#include "HatredChain_Body.h"
#include "HatredChain_Outter.h"
#include "Transform.h"

HatredChain::HatredChain(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 pos)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
	m_eSkillType = SKILL_TYPE::SKILL_DARKNESS2;
}

HatredChain::~HatredChain()
{
}

void HatredChain::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void HatredChain::BuildSkillEffects()
{
	SkillEff* pEff = new HatredChain_Body(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(0.f, 0.f, 0.f), this);
	m_vecSkillEff.push_back(pEff);
	pEff = new HatredChain_Outter(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
	//pEff = new HatredChain_Body(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(0.f, 0.f, 2.5f), this);
	//m_vecSkillEff.push_back(pEff);
	//pEff = new HatredChain_Body(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(0.f, 0.f, 4.5f), this);
	//m_vecSkillEff.push_back(pEff);

}

int HatredChain::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
	return 0;
}

void HatredChain::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void HatredChain::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
