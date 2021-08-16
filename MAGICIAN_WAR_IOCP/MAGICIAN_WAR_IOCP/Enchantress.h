#pragma once
#include "Skill.h"
class Enchantress :
    public Skill
{
public:
    Enchantress();
    virtual ~Enchantress();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);
public:
    virtual void MakeCollision();
private:
    float Value1;
};

