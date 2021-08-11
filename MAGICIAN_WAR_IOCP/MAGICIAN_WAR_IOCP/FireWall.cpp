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
    cout << "FireWall Update\n";
    return 0;
}

void FireWall::LateUpdate(const float& fTime)
{
    Skill::LateUpdate(fTime);
}
