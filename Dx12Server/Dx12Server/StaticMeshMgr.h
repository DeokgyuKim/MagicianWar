#pragma once
#include "include.h"

class StaticMeshMgr
{
private:
	StaticMeshMgr() {}
	StaticMeshMgr(const StaticMeshMgr& rhs) {}
	~StaticMeshMgr() {}
public:
	static StaticMeshMgr* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new StaticMeshMgr();
		return m_pInstance;
	}
	static void DestoryInstance() {
		if (m_pInstance != NULL)
		{
			m_pInstance->Release();
			delete m_pInstance;
		}
		m_pInstance = NULL;
	}
private:
	static StaticMeshMgr* m_pInstance;
private:
	void	Release();
public:
	HRESULT LoadMeshInfo(string strFilePath);
	multimap<string*, TransformStruct> GetMapMeshInfo() { return m_mmapInfo; }

private:
	multimap<string*, TransformStruct> m_mmapInfo;
};

