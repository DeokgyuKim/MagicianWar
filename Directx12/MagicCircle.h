#pragma once
#include "SkillEff.h"
class MagicCircle :
    public SkillEff
{
public:
    MagicCircle(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
        XMFLOAT3 pos = XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3 rot = XMFLOAT3(0.f, 90.f, 0.f), UINT idx = 0);
    ~MagicCircle();
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
    UINT m_iSkillIdx = 0;
};

