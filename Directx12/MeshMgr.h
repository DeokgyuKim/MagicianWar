#pragma once
#include "framework.h"

class MeshMgr
{
private:
	MeshMgr() {}
	MeshMgr(const MeshMgr& rhs) {}
	~MeshMgr() {}
public:
	static MeshMgr* GetInstnace()
	{
		if (m_pInstance == NULL)
			m_pInstance = new MeshMgr();
		return m_pInstance;
	}
	static void		DestroyInstance()
	{
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static MeshMgr* m_pInstance;

public:
	MeshInfo	GetMeshInfo(string meshName);
	int			GetVertexCnt(string meshName);
	int			GetIndexCnt(string meshName);

};

