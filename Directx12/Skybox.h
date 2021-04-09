#pragma once
#include "Object.h"

class Camera;
class Skybox :
    public Object
{
public:
    Skybox(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        string _meshName);
    ~Skybox();
private:
    void    Initialize();
    void    Release();

public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
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

