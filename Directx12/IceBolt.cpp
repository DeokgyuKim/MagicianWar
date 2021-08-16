#include "IceBolt.h"

#include "IceBolt_Ice.h"
#include "IceBolt_Body.h"
#include "IceBolt_Tornado.h"
//#include "IceBolt_Crevasses.h"

#include "Transform.h"



IceBolt::IceBolt(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos)
	: Skill(device, cmdLst, pRenderer)
{
	Initialize();
	dynamic_cast<Transform*>(GetTransController())->SetRotate(rotate);
	dynamic_cast<Transform*>(GetTransController())->SetPosition(pos);
	m_eSkillType = SKILL_TYPE::SKILL_COLD1;
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

	SkillEff* pEff = new IceBolt_Body(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);

	//pEff = new IceBolt_Ice(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//m_vecSkillEff.push_back(pEff);
	
	pEff = new IceBolt_Tornado(m_pDevice, m_pCmdLst, m_pRenderer, this);
	m_vecSkillEff.push_back(pEff);

	//pEff = new IceBolt_Crevasses(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//m_vecSkillEff.push_back(pEff);
	//dynamic_cast<Transform*>(pEff->GetComponent("Transform"))->SetPosition(XMFLOAT3(1.f, -2.3f, 0.7f));	

	//pEff = new IceBolt_Crevasses(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//dynamic_cast<Transform*>(pEff->GetComponent("Transform"))->SetRotate(XMFLOAT3(0.f, 120.f, 0.f));
	//m_vecSkillEff.push_back(pEff);

	//pEff = new IceBolt_Crevasses(m_pDevice, m_pCmdLst, m_pRenderer, this);
	//dynamic_cast<Transform*>(pEff->GetComponent("Transform"))->SetRotate(XMFLOAT3(0.f, 240.f, 0.f));
	//m_vecSkillEff.push_back(pEff);






}

int IceBolt::Update(const float& fTimeDelta)
{
	Skill::Update(fTimeDelta);

	//XMFLOAT3 playerPos = dynamic_cast<Transform*>(MainApp::GetInstance()->GetScene()->GetPlayer()->GetComponent("Transform"))->GetPosition();
	//dynamic_cast<Transform*>(GetTransController())->SetPosition(XMFLOAT3(playerPos.x, 2.f, playerPos.z));
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
