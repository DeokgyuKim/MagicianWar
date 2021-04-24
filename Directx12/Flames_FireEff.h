#pragma once
#include "SkillEff.h"
class Flames_FireEff :
    public SkillEff
{
public:
    Flames_FireEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
        Object* pParent = NULL, XMFLOAT3 pos = XMFLOAT3(0.f, 0.f, 0.f), UINT idx = 0);
    ~Flames_FireEff();
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

