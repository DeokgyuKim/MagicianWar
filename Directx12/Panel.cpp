#include "Panel.h"

#include "Transform.h"

Panel::Panel(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName)
    : UI(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName)
{
   // dynamic_cast<Transform*>(GetTransController())->SetScale(XMFLOAT3(0.f, 0.f, 0.f));
    m_fScale = 0.f;
}

Panel::~Panel()
{
}

int Panel::Update(const float& fTimeDelta)
{
    UI::Update(fTimeDelta);

    //m_fScale = min(m_fScale + fTimeDelta * 0.1f, 2.f);

    //dynamic_cast<Transform*>(GetTransController())->SetScale(XMFLOAT3(m_fScale, m_fScale, m_fScale));
   
    return 0;
}

void Panel::LateUpdate(const float& fTimeDelta)
{
    UI::LateUpdate(fTimeDelta);
}

void Panel::Render(const float& fTimeDelta, int _instanceCount)
{
    UI::Render(fTimeDelta);
}
