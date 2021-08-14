#include "BlackHole.h"

#include "BlackHole_Body.h"
#include "BlackHole_Inner.h"
#include "Transform.h"

BlackHole::BlackHole(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 pos)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
}

BlackHole::~BlackHole()
{
}

void BlackHole::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void BlackHole::BuildSkillEffects()
{
	//SkillEff* pEff = new BlackHole_Ice(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//m_vecSkillEff.push_back(pEff);
	SkillEff* pEff = new BlackHole_Body(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
	//pEff = new BlackHole_Tornado(m_pDevice, m_pCmdLst, m_pRenderer,XMFLOAT3(180.f,0.f,0.f), XMFLOAT3(0.06f, 0.06f, 0.06f), this);
	//m_vecSkillEff.push_back(pEff);
	//pEff = new BlackHole_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 110.f, 0.f), XMFLOAT3(0.06f, 0.06f, 0.06f), this);
	//m_vecSkillEff.push_back(pEff);
	//pEff = new BlackHole_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 220.f, 0.f), XMFLOAT3(0.06f, 0.06f, 0.06f), this);
	//m_vecSkillEff.push_back(pEff);
	pEff = new BlackHole_Inner(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);

	//pEff = new BlackHole_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, XMFLOAT3(180.f, 280.f, 0.f), this);
	//m_vecSkillEff.push_back(pEff);

}

int BlackHole::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
	return 0;
}

void BlackHole::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void BlackHole::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
