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
#include "Bar.h"

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

	if (m_pLoading != NULL)
	{
		m_pLoadingBar->SetRatio(XMFLOAT2(m_pLoading->GetCount() / 96.f, 1.f));
		//cout << m_pLoading->GetCount()<< endl;
	}

	bool CheckThread = m_pLoading->GetFinish();

	if (CheckThread)
	{
		if (!Core::GetInstance()->GetLoadingThreadExecute())
		{
			delete m_pLoading;
#ifdef NETWORK
			Network::GetInstance()->CallEvent(EVENT_LOADING_LOADINGEND, 0);
			Network::GetInstance()->CallEvent(EVENT_SCENE_CHANGE, 1, LOBBY_SCENE);
#else
			Network::GetInstance()->CallEvent(EVENT_SCENE_CHANGE, 1, LOBBY_SCENE);
#endif // !
			return -1;
		}
	}

	return 0;
}

void LoadingScene::Initialize()
{
	m_eSceneType = LOADING;

	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;


	Core::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pCamera = pCamera = dynamic_cast<Camera*>(pObj);
	Renderer::GetInstance()->SetCamera(dynamic_cast<Camera*>(pObj));
	m_pObjects[OBJ_CAMERA].push_back(pObj);

	TextureMgr::GetInstance()->BuildInitTextures(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, WINCX, WINCY), "MainLogo");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new Bar(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(15.f, WINCY - 75.f, WINCX - 30.f, 60.f), "LoadingBar");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pLoadingBar = dynamic_cast<Bar*>(pObj);
	m_pLoadingBar->SetTag(1);

	m_pLoading = new Loading(Core::GetInstance(), Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLstForLoading(), Renderer::GetInstance()->GetHeap());
}

