#pragma once
#include "SkillEff.h"
class IceBort_IceDorm :
    public SkillEff
{
public:
    IceBort_IceDorm(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        Object* pParent = NULL);
    ~IceBort_IceDorm();
private:
    // SkillEff��(��) ���� ��ӵ�
    virtual void BuildComponent() override;
    virtual void AddTexturesName() override;
    virtual void BuildShaders() override;
    void ReBuildShaders();
    virtual void BuildConstantBuffers() override;
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

private:

    bool m_bReBuildShader = false;
};

