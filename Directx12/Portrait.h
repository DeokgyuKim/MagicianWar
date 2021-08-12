#pragma once
#include "UI.h"

class Player;
class Portrait :
    public UI
{
public:
    Portrait(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, Player* pPlayer);
    virtual ~Portrait();
public:
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
protected:
    Player* m_pPlayer = nullptr;
};

