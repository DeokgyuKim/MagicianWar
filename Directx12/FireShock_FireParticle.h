#pragma once
#include "SkillEff.h"
class FireShock_FireParticle :
    public SkillEff
{
public:
    FireShock_FireParticle(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        Object* pParent = NULL);
    ~FireShock_FireParticle();
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
};

