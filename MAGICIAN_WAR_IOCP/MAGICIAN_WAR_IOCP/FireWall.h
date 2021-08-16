#pragma once
#include "Skill.h"
class FireWall :
    public Skill
{
public:
    FireWall();
    virtual ~FireWall();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);
private:
    void MakeCollision();
private:
    float Value1;
    PxRigidDynamic* m_pRigidDynamic = NULL;
};

