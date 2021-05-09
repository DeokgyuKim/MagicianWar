#include "LoadingScene.h"
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

#include "StaticMeshMgr.h"
#include "TextureMgr.h"

#include "Loading.h"

LoadingScene::LoadingScene()
{
	Initialize();
}

int LoadingScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);

	bool CheckThread = m_pLoading->GetFinish();

	if (CheckThread)
	{
		if (!Core::GetInstance()->GetLoadingThreadExecute())
		{
#ifdef NETWORK
			if (Network::GetInstance()->GetLobbyEnd())
			{
				delete m_pLoading;
				TestScene* pTestScene = new TestScene();
				MainApp::GetInstance()->ChangeScene(pTestScene);
				return -1;
			}
#else
			if (m_pButton->GetButtonState() == BUTTON_STATE::ON)
			{
				delete m_pLoading;
				TestScene* pTestScene = new TestScene();
				MainApp::GetInstance()->ChangeScene(pTestScene);
				return -1;
			}
#endif
		}
	}

	return 0;
}

void LoadingScene::Initialize()
{
	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;

	
	Core::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pCamera = pCamera = dynamic_cast<Camera*>(pObj);
	Renderer::GetInstance()->SetCamera(dynamic_cast<Camera*>(pObj));
	m_pObjects[OBJ_CAMERA].push_back(pObj);

	TextureMgr::GetInstance()->BuildInitTextures(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, WINCX, WINCY), "MainUi");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), 
		XMFLOAT4(1400.f, 700.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(0);

	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_pLoading = new Loading(Core::GetInstance(), Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLstForLoading(), Renderer::GetInstance()->GetHeap());

#ifdef NETWORK
	Network::GetInstance()->Init("192.168.219.109");
#endif
}

