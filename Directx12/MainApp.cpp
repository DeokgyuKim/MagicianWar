#include "MainApp.h"
#include "Core.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"
#include "KeyMgr.h"
#include "TestScene.h"
#include "LoadingScene.h"
#include "StaticMeshMgr.h"
#include "MaterialMgr.h"
#include "InstanceMgr.h"

MainApp* MainApp::m_pInstance = NULL;


void MainApp::Initialize()
{
	m_pCore = Core::GetInstance();
	m_pCore->InitDevice();
	
	m_MeshMgr = MeshMgr::GetInstnace();
	m_AnimationMgr = AnimationMgr::GetInstance();
	m_MaterialMgr = MaterialMgr::GetInstnace();
	m_MaterialMgr->InitMaterialMgr(m_pCore, m_pCore->GetDevice(), m_pCore->GetCmdLst());
	m_InstanceMgr = InstanceMgr::GetInstnace();
	m_InstanceMgr->InitInstanceMgr(m_pCore, m_pCore->GetDevice(), m_pCore->GetCmdLst());

	BuildMaterial();

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
}

void MainApp::Render(const float& fTimeDelta)
{
	if (m_iEvent == -1)
		return;
	m_pRenderer->Render(0.f);
	m_KeyMgr->KeyUpdate();
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

void MainApp::BuildMaterial()
{
	Material Default_Materials;
	Default_Materials.DiffuseAlbedo = XMFLOAT4(0.243f, 0.243f, 0.243f, 1.f);
	Default_Materials.Ambient = XMFLOAT3(0.243f, 0.243f, 0.243f);
	Default_Materials.Specular = XMFLOAT3(1.f, 1.f, 1.f);
	Default_Materials.Name = "Map_Materials";

	m_MaterialMgr->BuildMaterial("Map_Materials", Default_Materials);
}
