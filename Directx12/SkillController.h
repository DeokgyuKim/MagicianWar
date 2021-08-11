#pragma once
#include "UI.h"

class Scene;
class SkillController
{
private:
	SkillController();
	~SkillController();
public:
	static SkillController* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new SkillController();
		return m_pInstance;
	}
	static void DestroyInstance() {
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static SkillController* m_pInstance;

public:
	void SetSkillCnt(int idx, int cnt);
	int GetSkillCnt(int idx) { return m_iSkillCnt[idx]; }
public:
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene);
private:
	UI* m_pSkillBase[2];
	UI* m_pSkillOn[2][4];
	int m_iSkillCnt[2] = { 0, 0 };
};

