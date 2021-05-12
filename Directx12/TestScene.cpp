#include "TestScene.h"
#include "Core.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Skybox.h"
#include "StaticObject.h"
#include "Bullet.h"

#include "Flames.h"
#include "FireShock.h"
#include "FireRing.h"

#include "StaticMeshMgr.h"
#include "InstanceMgr.h"
#include "PoolingMgr.h"

#include "Network.h"

#include "Panel.h"

TestScene::TestScene()
{
	Initialize();
}

int TestScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);


	//if (Network::GetInstance()->GetGameEnd()->bEnd)
	//{
	//	cout << "°ÔÀÓ ³¡³µ¾î!!!!!" << endl;
	//	if (!GameEndForPanelCreate)
	//	{
	//		//Panel»ý¼º
	//		if (Network::GetInstance()->GetMyInfo().dwTeamNum == Network::GetInstance()->GetGameEnd()->teamNum)
	//		{
	//			m_pObjects[OBJ_UI].push_back(new Panel(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, 760.f, 200.f), "Win"));
	//		}
	//		else
	//		{
	//			m_pObjects[OBJ_UI].push_back(new Panel(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, 760.f, 200.f), "Lose"));
	//		}
	//		GameEndForPanelCreate = true;
	//	}
	//	if (Network::GetInstance()->GetCurScene() == 3)
	//	{
	//		//´ÙÀ½ ¾À »ý¼º ÈÄ ¹Ù²ãÁÜ.
	//	}
	//}

	return 0;
}

void TestScene::LateUpdate(const float& fTimeDelta)
{
	Scene::LateUpdate(fTimeDelta);

#ifdef NETWORK
	Object* pObj = GetPlayer();
	if (pObj != NULL) {
		Network::GetInstance()->SetMyPlayerInfo(dynamic_cast<Player*>(pObj));

	}

	PoolingMgr::GetInstance()->UpdatePoolingObject(Network::GetInstance()->GetBullets());
#endif
#ifdef PHYSX
	if (CPhysXMgr::GetInstance()->PhysXStartTime > 10.f)
	{
		CPhysXMgr::GetInstance()->gScene->simulate(fTimeDelta);
		CPhysXMgr::GetInstance()->gScene->fetchResults(true);
	}
	else
	{
		CPhysXMgr::GetInstance()->PhysXStartTime += fTimeDelta;
	}
#endif
}

void TestScene::Initialize()
{
	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;


	Core::GetInstance()->CmdLstReset();

#ifdef NETWORK
	XMFLOAT3 pos = Network::GetInstance()->GetMyPlayerStartPos();
	pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		CHARACTER_WIZARD_01, pos, Network::GetInstance()->GetMyInfo(),MESH_TYPE::CHARACTER);
	pPlayer = dynamic_cast<Player*>(pObj);
	m_pObjects[OBJ_PLAYER].push_back(pObj);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	map<DWORD, PlayerInfo> Others = Network::GetInstance()->GetOtherPlayerInfo();
	for (auto iter = Others.begin(); iter != Others.end(); ++iter)
	{
		Core::GetInstance()->CmdLstReset();
		pos = (*iter).second.xmfPosition;
		pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
			CHARACTER_WIZARD_01, pos, (*iter).second, MESH_TYPE::CHARACTER);
		Core::GetInstance()->CmdLstExecute();
		Core::GetInstance()->WaitForGpuComplete();
		m_pObjects[OBJ_PLAYER].push_back(pObj);
	}


#else
	pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		CHARACTER_WIZARD_01,MESH_TYPE::CHARACTER);
	pPlayer = dynamic_cast<Player*>(pObj);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
	m_pObjects[OBJ_PLAYER].push_back(pObj);
#endif
	
	
	Core::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pCamera = pCamera = dynamic_cast<Camera*>(pObj);
	m_pCamera->SetMode(CAMERA_MODE::CAMERA_THIRD);
	pPlayer->SetCamera(pCamera);
	dynamic_cast<Camera*>(pObj)->SetPlayer(pPlayer);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
	Renderer::GetInstance()->SetCamera(dynamic_cast<Camera*>(pObj));

	m_pObjects[OBJ_CAMERA].push_back(pObj);

	Core::GetInstance()->CmdLstReset();
	pObj = new Terrain(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		"StoneTerrain");
	//(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_pObjects[OBJ_STATIC].push_back(pObj);


	pObj = new Skybox(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		"SkyBox");
	dynamic_cast<Skybox*>(pObj)->SetCamera(pCamera);
	m_pObjects[OBJ_SKYBOX].push_back(pObj);

	//pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.01f, 0.01f, 0.01f), "overhang_building_a");
	//m_pObjects[OBJ_STATIC].push_back(pObj);
	//
	//pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT3(0.f, 0.f, 10.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.01f, 0.01f, 0.01f), "street_gate");
	//m_pObjects[OBJ_STATIC].push_back(pObj);

	StaticMeshMgr::GetInstance()->LoadMeshInfo("../Data/Map1Data.txt");
	//StaticMeshMgr::GetInstance()->LoadMeshInfo("../Data/Saved.txt");
	multimap<string*, TransformStruct> mmapInfo = StaticMeshMgr::GetInstance()->GetMapMeshInfo();
	for (auto iter = mmapInfo.begin(); iter != mmapInfo.end(); ++iter)
	{
		pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
			(*iter).second.xmfPosition, (*iter).second.xmfRotate, (*iter).second.xmfScale, *(*iter).first);
		m_pObjects[OBJ_STATIC].push_back(pObj);
	}

	//pObj = new Flames(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT3(10.f, 1.f, 5.f));
	//m_pObjects[OBJ_SKILL].push_back(pObj);
	//
	//pObj = new FireShock(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	//m_pObjects[OBJ_SKILL].push_back(pObj);

	//pObj = new FireRing(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	//m_pObjects[OBJ_SKILL].push_back(pObj);
	//
	//pObj = new Bullet(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), "FireBall",
	//	XMFLOAT3(20.f, 1.f, 30.f), XMFLOAT3(0.f, 180.f, 0.f), 3.f);
	//m_pObjects[OBJ_BULLET].push_back(pObj);

	//Core::GetInstance()->CmdLstReset();
	//pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	////(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());
	//Core::GetInstance()->CmdLstExecute();
	//Core::GetInstance()->WaitForGpuComplete();
	//
	//m_pObjects[OBJ_STATIC].push_back(pObj);
	
	PoolingMgr::GetInstance()->InitPoolingObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	//m_pObjects[OBJ_UI].push_back(new Panel(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), 
	//	XMFLOAT4(WINCX * 0.5f - 760.f * 0.75f, WINCY * 0.5f - 200.f * 0.75f, 760.f * 1.5f, 200.f * 1.5f), "Win"));
	m_pObjects[OBJ_UI].push_back(new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 20.f, WINCY * 0.5f - 20.f, 40.f, 40.f), "CrossHair"));
	
	BuildInstanceCBs();
	BuildMaterialCBs();
}
