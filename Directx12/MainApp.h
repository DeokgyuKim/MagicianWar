#pragma once
#include "framework.h"

class Scene;
class Core;
class MeshMgr;
class AnimationMgr;
class Renderer;
class KeyMgr;
class MainApp
{
public:
	MainApp() {}
	~MainApp() {}
public:
	void Initialize();
	void Update(const float& fTimeDelta);
	void LateUpdate(const float& fTimeDelta);
	void Render(const float& fTimeDelta);

private:
	void LoadSkinnedModels();
	void LoadStaticModels();
	void LoadAnimations();
private:
	Scene*			m_pScene;
	Core*			m_pCore;
	MeshMgr*		m_MeshMgr;
	AnimationMgr*	m_AnimationMgr;
	Renderer*		m_pRenderer;
	KeyMgr*			m_KeyMgr;
};

