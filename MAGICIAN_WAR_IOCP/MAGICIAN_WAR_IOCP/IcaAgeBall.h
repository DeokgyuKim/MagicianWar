#pragma once
#include "Skill.h"
class IcaAgeBall :
    public Skill
{
public:
    IcaAgeBall();
    virtual ~IcaAgeBall();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);

private:
    float Value1;
};

