#pragma once
#include "Object.h"

class Geometry;
class Camera;
class SkillEff;
class Skill :
    public Object
{
public:
    Skill(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~Skill();
protected:
    virtual void Initialize();
    void    Release();
protected:
    virtual void BuildComponents() PURE;
    virtual void BuildSkillEffects() PURE;
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
public:
    void SetSlotNum(unsigned char slot) { m_cSlotNum = slot; }
    unsigned char GetSlotNum() { return m_cSlotNum; }
    SKILL_TYPE GetSkillType() { return m_eSkillType; }

protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;
    SKILL_TYPE                                  m_eSkillType;
    unsigned char                               m_cSlotNum;
    vector<SkillEff*>                           m_vecSkillEff;
};

