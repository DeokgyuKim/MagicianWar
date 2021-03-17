#pragma once
#include "framework.h"

class Scene;
class Core;
class MeshMgr;
class Renderer;
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
	Scene*		m_pScene;
	Core*		m_pCore;
	MeshMgr*	m_MeshMgr;
	Renderer*	m_pRenderer;
};

