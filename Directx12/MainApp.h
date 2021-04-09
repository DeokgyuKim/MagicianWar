#pragma once
#include "framework.h"

class Scene;
class Core;
class MeshMgr;
class AnimationMgr;
class Renderer;
class KeyMgr;
class MaterialMgr;
class InstanceMgr;

class MainApp
{
private:
	MainApp() {}
	MainApp(const MainApp& rhs) {}
	~MainApp() { Release(); }
public:
	static MainApp* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new MainApp();
		return m_pInstance;
	}
	static void DestroyInstance() {
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static MainApp* m_pInstance;
public:
	void Initialize();
	void Update(const float& fTimeDelta);
	void LateUpdate(const float& fTimeDelta);
	void Render(const float& fTimeDelta);
	void Release();
public:
	Scene* GetScene() { return m_pScene; }
private:
	void LoadSkinnedModels();
	void LoadStaticModels();
	void LoadAnimations();
	void BuildMaterial();
private:
	Scene*			m_pScene;
	Core*			m_pCore;
	MeshMgr*		m_MeshMgr;
	AnimationMgr*	m_AnimationMgr;
	Renderer*		m_pRenderer;
	KeyMgr*			m_KeyMgr;
	MaterialMgr*	m_MaterialMgr;
	InstanceMgr*	m_InstanceMgr;
};

