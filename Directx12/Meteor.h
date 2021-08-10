#pragma once
#include "Skill.h"
class Meteor :
    public Skill
{
public:
    Meteor(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 rotate, XMFLOAT3 pos);
    ~Meteor();
private:
    virtual void BuildComponents() override;
    virtual void BuildSkillEffects() override;
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
};

