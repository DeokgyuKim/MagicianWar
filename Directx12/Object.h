#pragma once
#include "framework.h"

class Component;
class Renderer;


class Object
{
public:
	Object() {}
	Object(string _meshName);
	Object(string _meshName, MESH_TYPE _meshType);
	virtual ~Object();
public:
	virtual int		Update(const float& fTimeDelta);
	virtual void	LateUpdate(const float& fTimeDelta);
	virtual void	Render(const float& fTimeDelta, int _instanceCount = 1);
public:
	virtual string	GetTextureName() { return m_strTextureName; }
public: // Get
	string		GetInstName() const { return m_strInstName; }
	MESH_TYPE	GetMeshType() const { return m_strMeshType; }
	int			GetIndex() const { return m_Index; }
	UINT		GetMaterialIndex() { return m_MaterialIndex; }
	bool		GetIsInstancing() { return m_bInstanced; }

	virtual Component* GetTransController();
	Component* GetComponent(string strCom);
	//Set
	void		SetIndex(UINT index) { m_Index = index; }
public:
	void		SetPoolingValue(bool bPool, bool bPoolRender) { m_bPooling = bPool; m_bPoolRender = bPoolRender; }
	void		SetTextureName(string strTexture) { m_strTextureName = strTexture; }

protected:
	Renderer*							m_pRenderer;
	string								m_strTextureName = "";
	unordered_map<string, Component*>	m_mapComponent;
	bool								m_bInstanced = true;
	bool								m_bDead = false;

protected: // HBD
	string								m_strInstName = "";
	UINT								m_Index;
	MESH_TYPE							m_strMeshType = MESH_TYPE::COUNT;
	UINT								m_MaterialIndex = 0;

protected:	//Pool
	bool	m_bPooling = false;
	bool	m_bPoolRender = false;
};

