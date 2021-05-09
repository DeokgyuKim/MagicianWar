#include "FireRing.h"

#include "Component.h"
#include "Transform.h"


#include "FireRing_FireCylinder.h"

FireRing::FireRing(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
    : Skill(device, cmdLst, pRenderer)
{
    Initialize();
}

FireRing::~FireRing()
{
}

void FireRing::BuildComponents()
{
    Component* pComponent = new Transform(XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(20.f, 0.2f, 5.f));
    m_mapComponent["Transform"] = pComponent;
}

void FireRing::BuildSkillEffects()
{
    SkillEff* pEff = new FireRing_FireCylinder(m_pDevice, m_pCmdLst, m_pRenderer, this);
    m_vecSkillEff.push_back(pEff);

}

int FireRing::Update(const float& fTimeDelta)
{
    Skill::Update(fTimeDelta);
    return 0;
}

void FireRing::LateUpdate(const float& fTimeDelta)
{
    Skill::LateUpdate(fTimeDelta);
}

void FireRing::Render(const float& fTimeDelta, int _instanceCount)
{
    Skill::LateUpdate(fTimeDelta);
}
