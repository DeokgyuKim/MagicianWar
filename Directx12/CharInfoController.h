#pragma once
#include "CharInfo.h"

class Scene;
class CharInfoController
{
public:
    CharInfoController(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene);
    ~CharInfoController();
public:
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene);
public:
    void AddPlayer(int idx, Ui_Roomplayer* pPlayer);
private:
    vector<CharInfo*>   m_vecCharInfos;
    Scene*              m_pScene;
};

