#pragma once
#include "Scene.h"
#include "TextController.h"

class Camera;
class TestScene :
    public Scene
{
public:
    TestScene();
    virtual ~TestScene() {}
public:
    virtual int		Update(const float& fTimeDelta) override;
    virtual void    LateUpdate(const float& fTimeDelta) override;
private:
    void Initialize();
    void LateInit();
public:
    void SetGameEndForPanelCreate(bool _bCreate) { GameEndForPanelCreate = _bCreate; }
private:
    bool        ToolMode = false;
    Camera*     m_pCamera;
    bool        GameEndForPanelCreate = false;
    bool        m_LateInit = false;
    TextController* m_pTimeTextCtrl;
    TextController* m_pKillTextCtrl;
    float m_fGameTime = 0.f;
    int m_iOldMin, m_iOldSec;
    int m_iOldKill = 0;
};

