#pragma once
#include "Object.h"

class Geometry;
class Camera;
class SkillEff;
class Skill :
    public Object
{
public:
    Skill(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~Skill();
private:
    virtual void Initialize();
    void    Release();
private:
    virtual void BuildComponents() PURE;
    virtual void BuildSkillEffects() PURE;
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

protected:
    ID3D12Device*                               m_pDevice;
    ID3D12GraphicsCommandList*                  m_pCmdLst;

    vector<SkillEff*>                           m_vecSkillEff;
};

