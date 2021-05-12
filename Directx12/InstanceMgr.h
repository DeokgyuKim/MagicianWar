#pragma once
#include "framework.h"

class Core;
class Object;
class InstanceInfo;

class InstanceMgr
{
	InstanceMgr() {}
	InstanceMgr(const InstanceMgr& rhs) = delete;
	~InstanceMgr() {}
public:
	static InstanceMgr* GetInstnace()
	{
		if (m_pInstance == NULL)
			m_pInstance = new InstanceMgr();
		return m_pInstance;
	}
	static void		DestroyInstance()
	{
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static InstanceMgr* m_pInstance;
public:
	void InitInstanceMgr(Core* _core, ID3D12Device* _device,ID3D12GraphicsCommandList* _cmdList);
	
	void BuildInstanceBuffers();
	void UpdateInstanceBuffer(Object* _obj);
	void UpdateInstanceBuffer(UINT uiIdx, string instname, XMMATRIX world);

	void SetInstaneCount(string instname, UINT iCnt);
	

	void UpdateSkinnedBuffers(Object* _obj);

private:
	Core* m_pCore = NULL;
	ID3D12Device* m_pDevice = NULL;
	ID3D12GraphicsCommandList* m_pCmdLst = NULL;
public:

	// 모든 오브젝트들을 Mesh별로 나눠담음
	map<string, InstanceInfo*> m_InstanceObjects; 
	// 모든 오브젝트들의 Mesh별로 나뉜 CB
	map<string, unique_ptr<UploadBuffer<ObjectCB>>> m_InstanceCBs;
	// 모든 오브젝트들의 Bone별로 나뉜 CB
	map<string, unique_ptr<UploadBuffer<SkinnedCB>>> m_SkinnedCBs;
};

