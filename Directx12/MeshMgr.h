#pragma once
#include "framework.h"
#include "SkinnedModelInstance.h"


class Core;
class MaterialMgr;

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
	void InitMeshMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst);
	void BuildSkinnedModel(string meshName, MESH_TYPE etype);
	void BuildModel(string meshName, MESH_TYPE etype);

private: // mesh, skeleton Load
	bool LoadMeshFile(vector<SkinnedVertex>& outVertex, vector<UINT>& outIndex, vector<Material>* outMaterial,string path);
	bool LoadSkeletonFile(SkinnedData& outSkinnedData, string path);
	MaterialMgr* MaterialLoader;
public:
	MeshInfo	GetMeshInfo(string meshName);
	int			GetVertexCnt(string meshName);
	int			GetIndexCnt(string meshName);
public:
	// Get
	MeshGeometry* GetMesh(const string& meshName) { return m_Meshs[meshName].get(); }

private:
	Core* m_pCore = NULL;
	ID3D12Device* m_pDevice = NULL;
	ID3D12GraphicsCommandList* m_pCmdLst = NULL;

	// Mesh 정보
	unordered_map<string, unique_ptr<MeshGeometry>> m_Meshs;
	// 애니메이션 정보
	unordered_map <string, unique_ptr<SkinnedModelInstance>> m_SkinnedModelInst;
	string m_strFilePath;
};

