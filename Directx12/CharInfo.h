#pragma once
#include "UI.h"

class Scene;
class CharInfo :
    public UI
{
public:
    CharInfo(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strTextureName);
    virtual ~CharInfo();
public:
    virtual int Update(const float& fTimeDelta) override;
public:
    void SetRoomPlayer(Ui_Roomplayer* pPlayer);
    void PushPortraitInScene(Scene* pScene);
protected:
    Ui_Roomplayer*  m_pRoomPlayer;
    UI* m_pPortrait;
};

