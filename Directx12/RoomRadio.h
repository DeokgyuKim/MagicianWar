#pragma once
#include "RadioButton.h"

class Scene;
class TextController;
class RoomRadio :
    public RadioButton
{
public:
    RoomRadio(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
        XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName, Scene* pScene, int iTag);
    virtual ~RoomRadio();
public:
    virtual void LateUpdate(const float& fTimeDelta) override;
public:
    void SetScrollPos(float Scorll, float fRatio);
    void RemoveText();
protected:
    Scene* m_pScene;
    XMFLOAT4 m_xmfInfo;
    TextController* m_pTxtCtrl = NULL;
};

