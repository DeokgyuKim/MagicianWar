#include "Boom.h"

#include "Transform.h"
#include "Boom_Sphere.h"
#include "Boom_Inner.h"


Boom::Boom(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 goalScale, XMFLOAT3 pos, string ColorTexture)
	: Skill(device, cmdLst, pRenderer)
{
	m_strColorTex = ColorTexture;
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
	m_xmfGoalScale = goalScale;
}

Boom::~Boom()
{
}

void Boom::BuildComponents()
{
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
}

void Boom::BuildSkillEffects()
{
	//dynamic_cast<Transform*>(pEff->GetComponent("Transform"))->SetPosition(XMFLOAT3(1.f, -2.3f, 0.7f));	
	//m_vecSkillEff.push_back(pEff);

	//pEff = new Boom_Crevasses(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//dynamic_cast<Transform*>(pEff->GetComponent("Transform"))->SetRotate(XMFLOAT3(0.f, 120.f, 0.f));
	//m_vecSkillEff.push_back(pEff);

	//pEff = new Boom_Crevasses(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//dynamic_cast<Transform*>(pEff->GetComponent("Transform"))->SetRotate(XMFLOAT3(0.f, 240.f, 0.f));
	//m_vecSkillEff.push_back(pEff);

	SkillEff *pEff = new Boom_Sphere(m_pDevice, m_pCmdLst, m_pRenderer, 3.f, m_strColorTex, this);
	m_vecSkillEff.push_back(pEff);

	//pEff = new Boom_Inner(m_pDevice, m_pCmdLst, m_pRenderer, 1.f, this);
	//m_vecSkillEff.push_back(pEff);




}

int Boom::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);

	//XMFLOAT3 playerPos = dynamic_cast<Transform*>(MainApp::GetInstance()->GetScene()->GetPlayer()->GetComponent("Transform"))->GetPosition();
	//dynamic_cast<Transform*>(GetTransController())->SetPosition(XMFLOAT3(playerPos.x, 2.f, playerPos.z));
	return 0;
}

void Boom::LateUpdate(const float& fTimeDelta)
{
	Skill::LateUpdate(fTimeDelta);
}

void Boom::Render(const float& fTimeDelta, int _instanceCount)
{
	Skill::Render(fTimeDelta, _instanceCount);
}
