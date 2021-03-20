#pragma once
#include "Scene.h"

class Camera;
class TestScene :
    public Scene
{
public:
    TestScene();
    virtual ~TestScene() {}
public:
    virtual int		Update(const float& fTimeDelta);
private:
    void Initialize();
private:
    bool        ToolMode = false;
    Camera*     m_pCamera;
};

