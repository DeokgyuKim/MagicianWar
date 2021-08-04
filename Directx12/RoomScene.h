#pragma once
#include "Scene.h"

class Camera;
class Button;
class RadioButton;
class RoomScene :
    public Scene
{
public:
    RoomScene();

    virtual ~RoomScene() {}
public:
    virtual int		Update(const float& fTimeDelta);
private:
    void Initialize();
private:
    Camera* m_pCamera;
    Button* m_pButton;
    RadioButton* m_pRadio[3];
    RadioButton* m_pRadioTeam[2];
};

