#pragma once
#include "UI.h"
class Cursor :
    public UI
{
public:
    Cursor(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer);
    virtual ~Cursor();
public:
    virtual int Update(const float& fTimeDelta) override;
    virtual void LateUpdate(const float& fTimeDelta) override;
};

