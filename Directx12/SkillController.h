#pragma once
#include "UI.h"

class Scene;
class NetSkill;
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
	void SetSkillCoolTime(int idx, float fTime) { m_fSkillCoolTime[idx] = fTime; }
	void SetSkillCnt(int idx, int cnt);
	int GetSkillCnt(int idx) { return m_iSkillCnt[idx]; }
	bool UseSkill(int idx);
	XMFLOAT3 GeneratePositionForPacket(int idx);
	XMFLOAT3 GenerateRotateForPacket(int idx);
public:
	void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene, int CharType);
	void Update(const float& fTimeDelta);
private:
	UI* m_pSkillBase[2];
	UI* m_pSkillOn[2][4];
	int m_iSkillCnt[2] = { 0, 0 };
	float m_fSkillCoolTime[2] = { 0.f, 0.f };
	float m_fSkillCurCool[2] = { 0.f, 0.f };

	NetSkill* m_pNetSkill[2];
};

