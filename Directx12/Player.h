#pragma once
#include "Object.h"

class Buffer;
class Camera;

class Player :
    public Object
{
public:
    Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    ~Player();
private:
    void    Initialize();
    void    Release();
    HRESULT BuildConstantBuffer();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;

public:
    XMFLOAT3    GetPosition() { return m_xmfPosition; }
    void        SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

protected:
    ID3D12Device*                       m_pDevice;
    ID3D12GraphicsCommandList*          m_pCmdLst;
    unique_ptr<UploadBuffer<ObjectCB>>	m_ObjectCB;

    Buffer*                             m_pBuffer;

    XMFLOAT3                            m_xmfScale;
    XMFLOAT3							m_xmfRotate;
    XMFLOAT3                            m_xmfPosition;
    XMFLOAT4X4							m_xmmWorld;

    Camera*                             m_pCamera;
};

