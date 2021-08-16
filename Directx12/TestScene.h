#pragma once
#include "Scene.h"
#include "TextController.h"

class Camera;
class Player;
class TestScene :
    public Scene
{
public:
    TestScene();
    virtual ~TestScene() { Release(); }
public:
    virtual int		Update(const float& fTimeDelta) override;
    virtual void    LateUpdate(const float& fTimeDelta) override;
private:
    void Initialize();
    void LateInit();
    void Release();
public:
    void SetGameEndForPanelCreate(bool _bCreate) { GameEndForPanelCreate = _bCreate; }
    void MakeSkillForPacket(SKILL_TYPE etype, XMFLOAT3 pos, XMFLOAT3 rot, unsigned char slot);
    void UpdateSkillForPacket(SKILL_TYPE etype, XMFLOAT3 pos, XMFLOAT3 rot, unsigned char slot);
    void MakeSKillCircle(Player* pPlayer);
private:
    bool        ToolMode = false;
    Camera*     m_pCamera;
    bool        GameEndForPanelCreate = false;
    bool        m_LateInit = false;
    TextController* m_pTimeTextCtrl;
    TextController* m_pKillTextCtrl;
    short m_iGameTime = 0.f;
    int m_iOldMin, m_iOldSec;
    int m_iOldKill = 0;
};

