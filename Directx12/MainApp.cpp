#include "MainApp.h"
#include "Core.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"
#include "TestScene.h"

void MainApp::Initialize()
{
	m_pCore = Core::GetInstance();
	m_pCore->InitDevice();

	m_pCore->CmdLstReset();
	
	m_MeshMgr = MeshMgr::GetInstnace();
	m_AnimationMgr = AnimationMgr::GetInstance();
	LoadStaticModels();
	LoadSkinnedModels();
	LoadAnimations();

	
	
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

void MainApp::LoadSkinnedModels()
{
	// wizard_01
	m_MeshMgr->BuildSkinnedModel(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER);
	
}

void MainApp::LoadStaticModels()
{
	// Rocks
	m_MeshMgr->BuildModel(ROCK_02, MESH_TYPE::ROCK);
	// Trees
	m_MeshMgr->BuildModel(TREE_01, MESH_TYPE::TREE);
	// Houses
	m_MeshMgr->BuildModel(HOUSE_02, MESH_TYPE::HOUSE);
	// Tiles
	m_MeshMgr->BuildModel(TILE_01, MESH_TYPE::TILE);
}

void MainApp::LoadAnimations()
{
	// wizard_01
	m_AnimationMgr->BuildAnimation(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER, ANIMATION_TYPE::IDLE);
	m_AnimationMgr->BuildAnimation(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER, ANIMATION_TYPE::WALK);
	m_AnimationMgr->BuildAnimation(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER, ANIMATION_TYPE::DANCE);
}
