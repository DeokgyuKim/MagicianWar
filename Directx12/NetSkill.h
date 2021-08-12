#pragma once
#include "Component.h"

class Object;
class Camera;
enum SKILL_GENTYPE { SKILL_BULLET, SKILL_RAYCAST };
class NetSkill :
    public Component
{
public:
    NetSkill(Object* pOwner, Camera* Camera, SKILL_GENTYPE eSkillType);
    ~NetSkill();
public:
    XMFLOAT3 GetPosition();
    XMFLOAT3 GetRotate();
protected:
    Object* m_pOwner;
    Camera* m_pCamera;
    SKILL_GENTYPE m_eSkillType;
};

