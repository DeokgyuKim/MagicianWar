#pragma once
#include "Bar.h"

class Player;
class HpBar :
    public Bar
{
public:
    HpBar(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT4 xmfInfo, string strBaseTextureName);
    virtual ~HpBar();
public:
    // Object을(를) 통해 상속됨
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
    virtual void Render(const float& fTimeDelta, int _instanceCount = 1) override;
public:
    void SetPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }
protected:
    Player* m_pPlayer = NULL;
};

