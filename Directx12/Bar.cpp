#include "Bar.h"
#include "KeyMgr.h"

Bar::Bar(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName)
    : UI(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName)
{
}

Bar::~Bar()
{
}

int Bar::Update(const float& fTimeDelta)
{
    UI::Update(fTimeDelta);
   
    return 0;
}

void Bar::LateUpdate(const float& fTimeDelta)
{
    UI::LateUpdate(fTimeDelta);
}

void Bar::Render(const float& fTimeDelta, int _instanceCount)
{
    UI::Render(fTimeDelta);
}