#pragma once
#include "Text.h"

class Scene;
class TextController
{
public:
    TextController(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, const char* pText, Scene* pScene);
    virtual ~TextController();
public:
    void Initialize(XMFLOAT4 xmfInfo, const char* pText, Scene* pScene);
public:
    void UpdateChange();
    void SetScrollPos(float fRatio);
    void SetTextColor(XMFLOAT3 fColor);
public:
    void RemoveTexts(Scene* pScene);
private:
    void Release();
protected:
    ID3D12Device*               m_pDevice;
    ID3D12GraphicsCommandList*  m_pCmdLst;
    Renderer*                   m_pRenderer;
    Scene* m_pScene;
    int                         m_iTextLength;
    vector<Text*>               m_vecTxt;
};

