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
    float Value1;
};

