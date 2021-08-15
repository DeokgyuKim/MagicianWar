#pragma once
#include "SkillEff.h"
class Boom_Sphere :
    public SkillEff
{
public:
    Boom_Sphere(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, float goalScale, string ColorTexture,
        Object* pParent = NULL);
    ~Boom_Sphere();
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
    float m_fCurScale = 0.f;
    float m_fGoalScale = 1.f;
    string m_strColorTex = "";
};
