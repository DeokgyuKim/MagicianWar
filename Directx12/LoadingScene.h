#pragma once
#include "Scene.h"

class Loading;
class Camera;
class Button;
class LoadingScene :
    public Scene
{
public:
    LoadingScene();
    virtual ~LoadingScene() {}
public:
    virtual int		Update(const float& fTimeDelta);
private:
    void Initialize();
private:
    Loading*    m_pLoading = NULL;
    bool        ToolMode = false;
    Camera*     m_pCamera;
    Button*     m_pButton;
};

