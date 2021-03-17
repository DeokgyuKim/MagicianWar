#include "MainApp.h"
#include "Core.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "TestScene.h"

void MainApp::Initialize()
{
	m_pCore = Core::GetInstance();
	m_pCore->InitDevice();

	m_pCore->CmdLstReset();
	
	m_MeshMgr = MeshMgr::GetInstnace();
	m_MeshMgr->BuildSkinnedModel(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER);
	
	m_pCore->CmdLstExecute();
	m_pCore->WaitForGpuComplete();

	m_pRenderer = Renderer::GetInstance();

	m_pScene = new TestScene();
}

void MainApp::Update(const float& fTimeDelta)
{
	m_pScene->Update(fTimeDelta);
}

void MainApp::LateUpdate(const float& fTimeDelta)
{
	m_pScene->LateUpdate(fTimeDelta);
}

void MainApp::Render(const float& fTimeDelta)
{
	m_pRenderer->Render(0.f);
}
