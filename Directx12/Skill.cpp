#include "Skill.h"

#include "SkillEff.h"

Skill::Skill(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
    : Object()
{
    m_pDevice = device;
    m_pCmdLst = cmdLst;
    m_pRenderer = pRenderer;
    Initialize();
}

Skill::~Skill()
{
    Release();
}

void Skill::Initialize()
{
    BuildComponents();
    BuildSkillEffects();
}

void Skill::Release()
{
}

int Skill::Update(const float& fTimeDelta)
{
    Object::Update(fTimeDelta);
    for (auto iter = m_vecSkillEff.begin(); iter != m_vecSkillEff.end();)
    {
        if ((*iter)->Update(fTimeDelta) == -1)
        {
            delete (*iter);
            (*iter) = NULL;
            iter = m_vecSkillEff.erase(iter);
        }
        else
            ++iter;
    }
    return 0;
}

void Skill::LateUpdate(const float& fTimeDelta)
{
    Object::LateUpdate(fTimeDelta);
    for (int i = 0; i < m_vecSkillEff.size(); ++i)
        m_vecSkillEff[i]->LateUpdate(fTimeDelta);
}

void Skill::Render(const float& fTimeDelta, int _instanceCount)
{
    for (int i = 0; i < m_vecSkillEff.size(); ++i)
        m_vecSkillEff[i]->Render(fTimeDelta);
}
