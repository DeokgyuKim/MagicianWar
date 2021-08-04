#pragma once
#include "Scene.h"

class Camera;
class Button;
class RadioButton;
class CharInfoController;
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
    CharInfoController* m_pCharInfoCtrl;

    Ui_Roomplayer m_RoomPlayer[8];
};

