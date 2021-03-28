#pragma once

#include "framework.h"

class InterfaceAnimation
{
public:
	virtual void ChangeAnimation(ANIMATION_TYPE eType) abstract;
	virtual void LoopTime(const float _time) abstract;
};