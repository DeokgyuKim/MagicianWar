#pragma once
#include "SkillEff.h"
class IceBolt_Body :
    public SkillEff
{
public:
    IceBolt_Body(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        Object* pParent = NULL);
    ~IceBolt_Body();
private:
    // SkillEff��(��) ���� ��ӵ�
    virtual void BuildComponent() override;
    virtual void AddTexturesName() override;
    virtual void BuildShaders() override;
    virtual void BuildConstantBuffers() override;
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

private:
};

