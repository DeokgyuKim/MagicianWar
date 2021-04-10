#pragma once
#include "framework.h"
#include "Object.h"

#define TerrainX 1
#define TerrainZ 1

class Renderer;
class Geometry;

class Terrain :
    public Object
{
public:
    Terrain(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        string _meshName);
    ~Terrain();
private:
    void    Initialize();
    void    Release();

public:

public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;

protected:
    ID3D12Device*                           m_pDevice;
    ID3D12GraphicsCommandList*              m_pCmdLst;

    Geometry*                             m_pBuffer[TerrainZ][TerrainX];

    XMFLOAT3							m_xmfRotate;
    XMFLOAT4X4							m_xmmWorld;
};

