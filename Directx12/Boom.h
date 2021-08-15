#pragma once
#include "Skill.h"
class Boom :
    public Skill
{
public:
    Boom(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 goalScale, XMFLOAT3 pos, string ColorTexture);
    ~Boom();
private:
    virtual void BuildComponents() override;
    virtual void BuildSkillEffects() override;
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
private:
    XMFLOAT3 m_xmfGoalScale;
    string m_strColorTex;
};

