#pragma once
#include "Skill.h"
class Meteor :
    public Skill
{
public:
    Meteor();
    virtual ~Meteor();
public:
    virtual bool Update(const float& fTime);
    virtual void LateUpdate(const float& fTime);

private:
    float Value1;
};

