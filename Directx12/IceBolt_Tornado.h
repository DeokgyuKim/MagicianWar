#pragma once
#include "SkillEff.h"
class IceBolt_Tornado :
    public SkillEff
{
public:
    IceBolt_Tornado(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,XMFLOAT3 rotate = XMFLOAT3(0.f,0.f,0.f),
        Object* pParent = NULL);
    ~IceBolt_Tornado();
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

    XMFLOAT3 m_rotate;
};
