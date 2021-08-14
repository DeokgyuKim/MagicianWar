#pragma once
#include "Skill.h"
class IceFreeze :
    public Skill
{
public:
    IceFreeze(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos);
    ~IceFreeze();
private:
    virtual void BuildComponents() override;
    virtual void BuildSkillEffects() override;
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
};

