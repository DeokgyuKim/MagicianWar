#include "LobbyScene.h"
#include "TestScene.h"

#include "MainApp.h"
#include "Network.h"

#include "Core.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Skybox.h"
#include "StaticObject.h"
#include "Bullet.h"
#include "UI.h"
#include "Button.h"
#include "Bar.h"

#include "StaticMeshMgr.h"
#include "TextureMgr.h"

#include "Loading.h"

LobbyScene::LobbyScene()
{
	Initialize();
}

int LobbyScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);

	if (Network::GetInstance()->GetCurScene() == STAGE_Scene)
	{
		TestScene* Scene = new TestScene();
		MainApp::GetInstance()->ChangeScene(Scene);
		return -1;
	}

	return 0;
}

void LobbyScene::Initialize()
{
	m_eSceneType = LOBBY;
	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;


	Core::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pCamera = pCamera = dynamic_cast<Camera*>(pObj);
	Renderer::GetInstance()->SetCamera(dynamic_cast<Camera*>(pObj));
	m_pObjects[OBJ_CAMERA].push_back(pObj);
	Core::GetInstance()->WaitForGpuComplete();

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, WINCX, WINCY), "Lobby");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(1400.f, 800.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(0);


#ifdef NETWORK
	Network::GetInstance()->Init(Network::GetInstance()->LoadServerIPtxt("../Data/ServerIP.txt"));
#endif

}

