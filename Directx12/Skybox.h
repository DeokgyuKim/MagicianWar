#pragma once
#include "Object.h"

class Camera;
class Skybox :
    public Object
{
public:
    Skybox(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~Skybox();
private:
    void    Initialize();
    void    Release();
    HRESULT BuildConstantBuffer();
public:
    // Object��(��) ���� ��ӵ�
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;
public:
    void UpdateObjectCB();
public:
    void        SetCamera(Camera* camera) { m_pCamera = camera; }

protected:
    ID3D12Device* m_pDevice;
    ID3D12GraphicsCommandList* m_pCmdLst;

    unique_ptr<UploadBuffer<ObjectCB>>	        m_ObjectCB;

    Camera* m_pCamera;
};

