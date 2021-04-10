#pragma once
#include "framework.h"

class Component
{
public:
	Component() {}
	~Component() {}
public:
	virtual int		Update(const float& fTimeDelta) { return 0; }
	virtual void	LateUpdate(const float& fTimeDelta) {}
	virtual void	Render(const float& fTimeDelta, int _instanceCount = 1) {}
};

