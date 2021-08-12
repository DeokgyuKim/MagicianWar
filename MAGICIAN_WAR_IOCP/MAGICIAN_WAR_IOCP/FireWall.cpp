#include "FireWall.h"

FireWall::FireWall()
    :Skill()
{
   
}

FireWall::~FireWall()
{
}

int FireWall::Update(const float& fTime)
{
    Skill::Update(fTime);

    return 0;
}

void FireWall::LateUpdate(const float& fTime)
{
    Skill::LateUpdate(fTime);
}
