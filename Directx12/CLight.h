#pragma once
#include "framework.h"

class LightGeo;
class Renderer;
class CLight
{
public:
    CLight(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, Renderer* pRenderer,
        XMFLOAT4 diff, XMFLOAT4 ambi, XMFLOAT4 spec, XMFLOAT4 pos, XMFLOAT4 dir);
    virtual ~CLight();
private:
    void        Initialize();
    void        BuildConstantBuffer();
public:
    void        SetLightMaterial();
    void        RenderLight();
private:
    ID3D12Device*               m_pDevice;
    ID3D12GraphicsCommandList*  m_pCmdLst;
    ID3D12DescriptorHeap*       m_pDescriptorHeap;
    Renderer*                   m_pRenderer;

    unique_ptr<UploadBuffer<MatCB>>	    m_DiffuseCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_AmbientCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_SpecularCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_PositionCB;
    unique_ptr<UploadBuffer<MatCB>>	    m_DirectionCB;

	LightGeo*   m_pLightGeo;

    XMFLOAT4    m_xmfDiffuse;
    XMFLOAT4    m_xmfAmbient;
    XMFLOAT4    m_xmfSpecular;

    XMFLOAT4    m_xmfPosition;
    XMFLOAT4    m_xmfDirection;
};

