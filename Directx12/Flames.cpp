#include "Flames.h"

#include "Component.h"
#include "Transform.h"

#include "Flames_FireEff.h"

Flames::Flames(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotation)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(xmfPosition);
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(xmfRotation);
	m_eSkillType = SKILL_TYPE::SKILL_FIRE1;
}

Flames::~Flames()
{
}

void Flames::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void Flames::BuildSkillEffects()
{
	SkillEff* pEff = new Flames_FireEff(m_pDevice, m_pCmdLst, m_pRenderer, this, XMFLOAT3(0.f, 2.f, 0.01f), 3);
	m_vecSkillEff.push_back(pEff);
	pEff = new Flames_FireEff(m_pDevice, m_pCmdLst, m_pRenderer, this, XMFLOAT3(0.f, 2.f, -0.01f), 2);
	m_vecSkillEff.push_back(pEff);
}

int Flames::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);
	m_fMake = min(m_fMake + fTimeDelta * 4.f, 2.f);
	

	for (auto skilleff : m_vecSkillEff)
	{
		XMFLOAT3 pos = dynamic_cast<Transform*>(skilleff->GetTransController())->GetPosition();
		pos.y = m_fMake;
		dynamic_cast<Transform*>(skilleff->GetTransController())->SetPosition(pos);
	}

	return 0;
}

void Flames::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void Flames::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta);
}
