#pragma once
#include "framework.h"
#include "Object.h"

class LightGeo;
class Renderer;
class UI : public Object
{
public:
    UI(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strTextureName);
    virtual ~UI();
private:
    void        Initialize(XMFLOAT4 xmfInfo);
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta) override;
private:
    ID3D12Device*               m_pDevice;
    ID3D12GraphicsCommandList*  m_pCmdLst;
    Renderer*                   m_pRenderer;
};

