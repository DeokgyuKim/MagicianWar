#pragma once
#include "SkillEff.h"
class Boom_Inner :
    public SkillEff
{
public:
    Boom_Inner(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, float goalScale, Object* pParent = NULL);
    ~Boom_Inner();
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
    UINT m_iSkillIdx = 0;
    float m_fCurScale = 0.f;
    float m_fGoalScale;
};

