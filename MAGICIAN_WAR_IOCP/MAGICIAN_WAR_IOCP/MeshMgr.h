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
	void InitMeshMgr();
	void BuildModel(string meshName);
	void BuildModels();
	//void Build

private:
	bool LoadStaticMeshFile(vector<Vertex>& outVertex,
		vector<uint32_t>& outIndex, vector<Material>* outMaterial,
		string path);
public:
	// Get
	//MeshGeometry* GetMesh(const string& meshName) { return m_Meshs[meshName].get(); }
	MeshVtxInfo GetMeshVtxInfo(const string& meshName) { return m_StaticMeshVtxInfo[meshName]; }
	unordered_map<string, MeshVtxInfo> GetMeshVtxInfos() { return m_StaticMeshVtxInfo; }
private:
	string m_strFilePath;

	unordered_map<string, MeshVtxInfo>	m_StaticMeshVtxInfo;

};

