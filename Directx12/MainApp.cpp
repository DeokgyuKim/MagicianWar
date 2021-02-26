#include "MainApp.h"
#include "Core.h"
#include "Renderer.h"
#include "TestScene.h"

void MainApp::Initialize()
{
	m_pCore = Core::GetInstance();
	m_pCore->InitDevice();

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
