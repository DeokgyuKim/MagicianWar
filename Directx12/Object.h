#pragma once
#include "framework.h"

class Renderer;
class Object
{
public:
	Object() {}
	virtual ~Object() {}
public:
	virtual int		Update(const float& fTimeDelta)		PURE;
	virtual void	LateUpdate(const float& fTimeDelta)	PURE;
	virtual void	Render(const float& fTimeDelta)		PURE;
protected:
	Renderer*	m_pRenderer;
};

