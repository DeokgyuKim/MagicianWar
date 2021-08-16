#include "IceCrevasses.h"

#include "IceBolt_Crevasses.h"
#include "Transform.h"

IceCrevasses::IceCrevasses(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos)
    : Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetRotate(rotate);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
	m_eSkillType = SKILL_TYPE::SKILL_CRESSVAS;
}

IceCrevasses::~IceCrevasses()
{
}

void IceCrevasses::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void IceCrevasses::BuildSkillEffects()
{
	SkillEff* pEff = new IceBolt_Crevasses(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
}

int IceCrevasses::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
    return 0;
}

void IceCrevasses::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void IceCrevasses::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
