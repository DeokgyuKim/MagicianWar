#pragma once
#include "Scene.h"

class Loading;
class Camera;
class Button;
class RadioButton;
class Bar;
class LobbyScene :
    public Scene
{
public:
    LobbyScene(bool bRetry = false);
    
    virtual ~LobbyScene() {}
public:
    virtual int		Update(const float& fTimeDelta);
private:
    void Initialize(bool bRetry);
private:
    Camera*         m_pCamera;
    Button*         m_pButton;
    RadioButton*    m_pRadio[3];
    int iTag = BUTTON_ROOM_END;
};

