#include "FireShock.h"

#include "Component.h"
#include "Transform.h"

#include "FireShock_FireCylinder.h"
#include "FireShock_FireParticle.h"

FireShock::FireShock(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
}

FireShock::~FireShock()
{
}

void FireShock::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(3.f, 3.f, 10.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(20.f, 1.f, 10.f));
	m_mapComponent["Transform"] = pComponent;
}

void FireShock::BuildSkillEffects()
{
	SkillEff* pEff = new FireShock_FireCylinder(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
	pEff = new FireShock_FireParticle(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
}

int FireShock::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
	return 0;
}

void FireShock::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void FireShock::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta);
}
