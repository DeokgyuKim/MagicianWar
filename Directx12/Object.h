#pragma once
#include "framework.h"

class Component;
class Renderer;

class Object
{
public:
	Object() {}
	virtual ~Object();
public:
	virtual int		Update(const float& fTimeDelta);
	virtual void	LateUpdate(const float& fTimeDelta);
	virtual void	Render(const float& fTimeDelta);
public:
	string		GetTextureName() { return m_strTextureName; }
public: // HBD
	string		GetMeshName() const { return m_strMeshName; }
	string		GetInstID() const { return m_strInstID; }
	UINT		GetInstIndex() const { return m_instIndex; }
protected:
	Renderer*							m_pRenderer;
	string								m_strTextureName = "";
	unordered_map<string, Component*>	m_mapComponent;


protected: // HBD
	string								m_strMeshName = "";
	string								m_strInstID;
	UINT								m_instIndex;
};

