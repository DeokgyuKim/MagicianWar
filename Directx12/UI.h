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
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
public:
    int         GetTag() { return m_iTag; }
    string      GetTextTextureName() { return m_strTextTextureName; }
    void        SetTag(const int& iTag) { m_iTag = iTag; }
public:
    void    SetRatio(XMFLOAT2 xmfRatio) { m_xmfRatio = xmfRatio; }
    void    SetTextTextureName(string strName) { m_strTextTextureName = strName; }
protected:
    ID3D12Device*               m_pDevice;
    ID3D12GraphicsCommandList*  m_pCmdLst;
    Renderer*                   m_pRenderer;
    int                         m_iTag = -1;
    unique_ptr<UploadBuffer<UiCB>>	        m_UiCB;
    XMFLOAT2 m_xmfRatio = XMFLOAT2(1.f, 1.f);
    string m_strTextTextureName = "";
};

