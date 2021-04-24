#pragma once
#include "framework.h"
#include "Object.h"

class Shader;
class SkillEff :
    public Object
{
public:
    SkillEff(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Object* pParent = NULL);
    ~SkillEff();
protected:
    void    Initialize();
    void    Release();
protected:
    //Create Component, Set Options, and Add m_mapComponent
    virtual void    BuildComponent() PURE;
    //Push Texture Name at m_lstTextureName in order
    virtual void    AddTexturesName() PURE;
    //Create Shader at m_pShader;
    virtual void    BuildShaders() PURE;
    //Create ObjectCB and SkillCB;
    virtual void    BuildConstantBuffers() PURE;
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
    float m_fTime = 0.f;
};

