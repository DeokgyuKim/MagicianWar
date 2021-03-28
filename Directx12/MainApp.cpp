#include "MainApp.h"
#include "Core.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"
#include "TestScene.h"
#include "StaticMeshMgr.h"

MainApp* MainApp::m_pInstance = NULL;

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

void MainApp::Release()
{
	StaticMeshMgr::DestoryInstance();
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
	
	m_MeshMgr->BuildModel("barrel_group", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("box", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("castle_door_1", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("castle_tower_octagon", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("castle_tower_round", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("citadel_base_a", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("city_tower_a", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_18_a", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_18_b", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_36_c", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_37_e", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_37_h", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_46", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("civilian_house_ruin_e", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("fountain_a", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("overhang_building_a", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("roof_building_4", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("roof_building_5", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("roof_building_11", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("roof_building_12", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("street_gate", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("tower_gates", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("wall_H10m_L15m", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("wall_H10m_L50m", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("wagon", MESH_TYPE::TILE);
	m_MeshMgr->BuildModel("Sphere", MESH_TYPE::TILE);
}

void MainApp::LoadAnimations()
{
	// wizard_01
	m_AnimationMgr->BuildAnimation(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER, ANIMATION_TYPE::IDLE);
	m_AnimationMgr->BuildAnimation(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER, ANIMATION_TYPE::WALK);
	m_AnimationMgr->BuildAnimation(CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER, ANIMATION_TYPE::DANCE);
}
