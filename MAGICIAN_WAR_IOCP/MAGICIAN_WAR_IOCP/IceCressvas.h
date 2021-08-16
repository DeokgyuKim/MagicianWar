#pragma once
#include "Skill.h"
class IceCressvas :
    public Skill
{
public:
    IceCressvas();
    virtual ~IceCressvas();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);

private:
    float Value1;
};

