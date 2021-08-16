#pragma once
#include "Skill.h"
class DistortionPearl :
    public Skill
{
public:
    DistortionPearl();
    virtual ~DistortionPearl();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);
public:
    virtual void MakeCollision();
private:
    float Value1;
};

