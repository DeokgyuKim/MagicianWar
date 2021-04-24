#pragma once
#include "framework.h"

class Component;
class Renderer;


class Object
{
public:
	Object() {}
	Object(string _meshName);
	virtual ~Object();
public:
	virtual int		Update(const float& fTimeDelta);
	virtual void	LateUpdate(const float& fTimeDelta);
	virtual void	Render(const float& fTimeDelta, int _instanceCount = 1);
public:
	virtual string	GetTextureName() { return m_strTextureName; }
public: // Get
	string		GetMeshName() const { return m_strMeshName; }
	int			GetIndex() const { return m_Index; }
	UINT		GetMaterialIndex() { return m_MaterialIndex; }
	bool		GetIsInstancing() { return m_bInstanced; }

	virtual Component* GetTransController();

protected:
	Renderer*							m_pRenderer;
	string								m_strTextureName = "";
	unordered_map<string, Component*>	m_mapComponent;
	bool								m_bInstanced = true;


protected: // HBD
	string								m_strMeshName = "";
	UINT								m_Index;
	UINT								m_MaterialIndex = 0;

	
};

