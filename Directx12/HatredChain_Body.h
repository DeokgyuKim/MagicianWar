#pragma once
#include "SkillEff.h"
class HatredChain_Body :
    public SkillEff
{
public:
    HatredChain_Body(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT3 pos,
        Object* pParent = NULL);
    ~HatredChain_Body();
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
