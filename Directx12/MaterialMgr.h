#pragma once
#include "framework.h"

class Core;

class MaterialMgr
{
private:
	MaterialMgr() {}
	MaterialMgr(const MaterialMgr& rhs) {}
	~MaterialMgr() {}
public:
	static MaterialMgr* GetInstnace()
	{
		if (m_pInstance == NULL)
			m_pInstance = new MaterialMgr();
		return m_pInstance;
	}
	static void		DestroyInstance()
	{
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static MaterialMgr* m_pInstance;

public:
	// Loop
	void UpdateMaterialCBs();
public:
	void InitMaterialMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst);
	void BuildMaterial(const string& MaterialName, Material Mat);
	void BuildMaterialCBs();
	// Set
	void SetGraphicsShaderResourceView();
	
public:
	Material* GetMaterial(const string& MaterialName);
private:
	Core* m_pCore = NULL;
	ID3D12Device* m_pDevice = NULL;
	ID3D12GraphicsCommandList* m_pCmdLst = NULL;


	unordered_map<string, unique_ptr<Material>> m_Materials;
	unique_ptr<UploadBuffer<MaterialCB>>		m_MaterialCBs;
	int MaterialCount = -1;
	
};

