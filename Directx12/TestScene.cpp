#include "TestScene.h"
#include "Core.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"

TestScene::TestScene()
{
	Initialize();
}

void TestScene::Initialize()
{
	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;

	//Core::GetInstance()->CmdLstReset();
	//pObj = new Triangle(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst());
	//Core::GetInstance()->CmdLstExecute();
	//Core::GetInstance()->WaitForGpuComplete();
	//
	//m_pObjects[OBJ_PLAYER].push_back(pObj);
	Core::GetInstance()->CmdLstReset();
	pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	pPlayer = dynamic_cast<Player*>(pObj);
	//(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
	
	m_pObjects[OBJ_PLAYER].push_back(pObj);

	Core::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	pCamera = dynamic_cast<Camera*>(pObj);
	pPlayer->SetCamera(pCamera);
	dynamic_cast<Camera*>(pObj)->SetPlayer(pPlayer);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
	Renderer::GetInstance()->SetCamera(dynamic_cast<Camera*>(pObj));

	m_pObjects[OBJ_CAMERA].push_back(pObj);

	Core::GetInstance()->CmdLstReset();
	pObj = new Terrain(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	//(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_pObjects[OBJ_PLAYER].push_back(pObj);

}
