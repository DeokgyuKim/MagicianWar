#pragma once
#include "Skill.h"
class IceFreeze :
    public Skill
{
public:
    IceFreeze();
    virtual ~IceFreeze();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);

private:
    float Value1;
};