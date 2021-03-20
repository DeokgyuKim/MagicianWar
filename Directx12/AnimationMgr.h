#pragma once
#include "framework.h"
#include "SkinnedModelInstance.h"

class Core;

class AnimationMgr
{
private:
	AnimationMgr() {}
	AnimationMgr(const AnimationMgr& rhs) = delete;
	~AnimationMgr() {}
public:
	static AnimationMgr* GetInstance()
	{
		if (m_pInstance == NULL)
			m_pInstance = new AnimationMgr();
		return m_pInstance;
	}
	static void DestoryInstance()
	{
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static AnimationMgr* m_pInstance;

public:
	void InitAnimationMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst);
	void BuildAnimation(string meshName, MESH_TYPE eMesh, ANIMATION_TYPE eAnimation);
	void SetSkinnedDatas(string meshName, unique_ptr<SkinnedData> _skinnedinfo);
public:
	SkinnedModelInstance* GetAnimations(const string& ObjectName) { return m_SkinnedModelInsts[ObjectName].get(); }
private:
	bool LoadAnimationFile(const ANIMATION_TYPE& eAnimation);

private:
	Core* m_pCore = NULL;
	ID3D12Device* m_pDevice = NULL;
	ID3D12GraphicsCommandList* m_pCmdLst = NULL;

	string m_meshName = "";
	// 애니메이션 정보
	unordered_map <string, unique_ptr<SkinnedModelInstance>> m_SkinnedModelInsts;
	
	string m_strFilePath;
};

