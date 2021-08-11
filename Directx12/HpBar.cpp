#include "HpBar.h"
#include "Player.h"

HpBar::HpBar(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName)
    : Bar(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName)
{
}

HpBar::~HpBar()
{
}

int HpBar::Update(const float& fTimeDelta)
{
    Bar::Update(fTimeDelta);

    if (m_pPlayer != nullptr)
    {
        SetRatio(XMFLOAT2(m_pPlayer->GetNetworkInfo().iHp / 100.f, 1.f));
    }

    return 0;
}

void HpBar::LateUpdate(const float& fTimeDelta)
{
    Bar::LateUpdate(fTimeDelta);
}

void HpBar::Render(const float& fTimeDelta, int _instanceCount)
{
    Bar::Render(fTimeDelta);
}