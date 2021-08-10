#pragma once
#include "SkillEff.h"
class Meteor_Body :
    public SkillEff
{
public:
    Meteor_Body(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        Object* pParent = NULL);
    ~Meteor_Body();
private:
    // SkillEff을(를) 통해 상속됨
    virtual void BuildComponent() override;
    virtual void AddTexturesName() override;
    virtual void BuildShaders() override;
    virtual void BuildConstantBuffers() override;
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

private:
};

