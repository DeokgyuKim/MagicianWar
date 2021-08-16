#include "IceFreeze.h"

#include "IceFreeze_IceDorm.h"
#include "Transform.h"
#include "SoundMgr.h"


IceFreeze::IceFreeze(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos)
    : Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetRotate(rotate);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
	m_eSkillType = SKILL_TYPE::SKILL_COLD2;
	SoundMgr::GetInstance()->PlaySound("IceFreezeMake");
}

IceFreeze::~IceFreeze()
{
	SoundMgr::GetInstance()->PlaySound("IceFreezeDestroy");
}

void IceFreeze::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void IceFreeze::BuildSkillEffects()
{
	SkillEff* pEff = new IceFreeze_IceDorm(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);
}

int IceFreeze::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
    return 0;
}

void IceFreeze::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void IceFreeze::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
