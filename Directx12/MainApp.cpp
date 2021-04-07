#include "MainApp.h"
#include "Core.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"
#include "KeyMgr.h"
#include "TestScene.h"
#include "LoadingScene.h"
#include "StaticMeshMgr.h"

MainApp* MainApp::m_pInstance = NULL;


void MainApp::Initialize()
{
	m_pCore = Core::GetInstance();
	m_pCore->InitDevice();

	m_MeshMgr = MeshMgr::GetInstnace();
	m_AnimationMgr = AnimationMgr::GetInstance();

	m_pRenderer = Renderer::GetInstance();
	m_KeyMgr = KeyMgr::GetInstance();
	m_pScene = new LoadingScene();
}

void MainApp::Update(const float& fTimeDelta)
{
	m_iEvent = m_pScene->Update(fTimeDelta);
}

void MainApp::LateUpdate(const float& fTimeDelta)
{
	if (m_iEvent == -1)
		return;
	m_pScene->LateUpdate(fTimeDelta);
	m_KeyMgr->KeyUpdate();
}

void MainApp::Render(const float& fTimeDelta)
{
	if (m_iEvent == -1)
		return;
	m_pRenderer->Render(0.f);
}

void MainApp::Release()
{
	StaticMeshMgr::DestoryInstance();
}

void MainApp::ChangeScene(Scene* pScene)
{
	delete m_pScene;
	m_pScene = pScene;
}
