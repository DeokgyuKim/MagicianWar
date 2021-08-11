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
private:
    bool        ToolMode = false;
    Camera*     m_pCamera;
    bool        GameEndForPanelCreate = false;
    TextController* m_pTextCtrl;
    float m_fGameTime = 0.f;
    int m_iOldMin, m_iOldSec;
};

