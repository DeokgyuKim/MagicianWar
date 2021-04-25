#pragma once
#include "framework.h"

#include "Skill.h"
class FireShock :
    public Skill
{
public:
    FireShock(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~FireShock();
private:
    virtual void BuildComponents() override;
    virtual void BuildSkillEffects() override;
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
};

