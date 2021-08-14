#pragma once
#include "framework.h"
#include "SkillEff.h"

class Shader;
class BillBoardEff :
    public SkillEff
{
public:
    BillBoardEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent = NULL);
    ~BillBoardEff();
protected:
    void    Initialize();
    void    Release();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
public:


protected:
    ID3D12Device* m_pDevice;
    ID3D12GraphicsCommandList* m_pCmdLst;

    list<Object*>                               m_lstObjects;
    vector<string>                              m_lstTextureName;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB;
    unique_ptr<UploadBuffer<SkillCB>>	        m_SkillCB;
    Shader*                                     m_pShader = NULL;
    Object*                                     m_pParent = NULL;
    RENDER_TYPE                                 m_eRenderType = RENDER_POSTSKILL;
    float m_fTime = 0.f;
};

