#include "TestScene.h"
#include "Core.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Skybox.h"
#include "StaticObject.h"
#include "Bullet.h"
#include "HpBar.h"
#include "Cursor.h"

#include "SkillController.h"
#include "ShopController.h"

#include "Flames.h"
#include "FireShock.h"
#include "FireRing.h"
#include "IceBolt.h"
#include "Meteor.h"

#include "StaticMeshMgr.h"
#include "InstanceMgr.h"
#include "PoolingMgr.h"

#include "Network.h"

#include "Panel.h"
#include "LobbyScene.h"
#include "MainApp.h"

TestScene::TestScene()
{
	Initialize();
}

int TestScene::Update(const float& fTimeDelta)
{
	LateInit();
	Scene::Update(fTimeDelta);

	m_iGameTime = Network::GetInstance()->GetRoundTime();

	int min = int(round(m_iGameTime)) / 60;
	int sec = int(round(m_iGameTime)) % 60;
	string Time;
	Time += to_string(min);
	Time += " ";
	Time += to_string(sec);

	if (m_iOldMin != min || m_iOldSec != sec)
	{
		m_pTimeTextCtrl->RemoveTexts(this);
		m_pTimeTextCtrl->Initialize(XMFLOAT4(WINCX / 2.f, 10.f, 50.f, 50.f), Time.c_str(), this);
	}
	m_iOldMin = min;
	m_iOldSec = sec;

	int kill = dynamic_cast<Player*>(GetPlayer())->GetKillCount();
	if (m_iOldKill != kill)
	{
		m_pKillTextCtrl->RemoveTexts(this);
		m_pKillTextCtrl->Initialize(XMFLOAT4(WINCX - 240.f, 10.f, 100.f, 100.f), to_string(kill).c_str(), this);
	}
	

	if (Network::GetInstance()->GetRoundEnd().WinnerTeam != TEAM_NONE)
	{
		//cout << "���� ������!!!!!" << endl;
		if (!GameEndForPanelCreate)
		{
			//Panel����
			UI* pPanel = nullptr;
			if (Network::GetInstance()->GetMyInfo().TeamType == Network::GetInstance()->GetRoundEnd().WinnerTeam)
			{
				pPanel = new Panel(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
					XMFLOAT4(WINCX * 0.5f - 760.f * 0.75f, 50.f, 760.f * 1.5f, 200.f * 1.5f), "Win");
				pPanel->SetTag(99);
				m_pObjects[OBJ_UI].push_back(pPanel);
			}
			else
			{
				pPanel = new Panel(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
					XMFLOAT4(WINCX * 0.5f - 760.f * 0.75f, 50.f, 760.f * 1.5f, 200.f * 1.5f), "Lose");
				pPanel->SetTag(99);
				m_pObjects[OBJ_UI].push_back(pPanel);
			}
			GameEndForPanelCreate = true;
		}

	}
	//if (Network::GetInstance()->GetCurScene() == LOBBY_Scene)
	//{
	//	//���� �� ���� �� �ٲ���.
	//	LobbyScene* lobbyScene = new LobbyScene(true);
	//	MainApp::GetInstance()->ChangeScene(lobbyScene);
	//	return -1;
	//}

	ShopController::GetInstance()->Update();

	return 0;
}

void TestScene::LateUpdate(const float& fTimeDelta)
{
	Scene::LateUpdate(fTimeDelta);

#ifdef NETWORK


	PoolingMgr::GetInstance()->UpdatePoolingObject(Network::GetInstance()->GetBullets());
#endif

#ifdef PHYSX
	//if (CPhysXMgr::GetInstance()->PhysXStartTime > 10.f)
	//{
	//	CPhysXMgr::GetInstance()->gScene->simulate(fTimeDelta);
	//	CPhysXMgr::GetInstance()->gScene->fetchResults(true);
	//}
	//else
	//{
	//	CPhysXMgr::GetInstance()->PhysXStartTime += fTimeDelta;
	//}
#endif
}

void TestScene::Initialize()
{
	//ShowCursor(FALSE);
	m_eSceneType = MAIN;
	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;




#ifdef NETWORK
	XMFLOAT3 pos = Network::GetInstance()->GetMyPlayerStartPos();
	switch (Network::GetInstance()->GetMyInfo().CharacterType)
	{
	case ELEMENT_FIRE:
		pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
			CHARACTER_WIZARD_FIRE, pos, Network::GetInstance()->GetMyInfo(), MESH_TYPE::CHARACTER);
		break;
	case ELEMENT_COLD:
		pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
			CHARACTER_WIZARD_COLD, pos, Network::GetInstance()->GetMyInfo(), MESH_TYPE::CHARACTER);
		break;
	case ELEMENT_DARKNESS:
		pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
			CHARACTER_WIZARD_DARKNESS, pos, Network::GetInstance()->GetMyInfo(), MESH_TYPE::CHARACTER);
		break;
	}
	pPlayer = dynamic_cast<Player*>(pObj);
	m_pObjects[OBJ_PLAYER].push_back(pObj);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	map<int, PlayerInfo> Others = Network::GetInstance()->GetOtherPlayerInfo();
	for (auto iter = Others.begin(); iter != Others.end(); ++iter)
	{
		Core::GetInstance()->CmdLstReset();
		pos = (*iter).second.xmfPosition;
		switch ((*iter).second.CharacterType)
		{
		case ELEMENT_FIRE:
			pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
				CHARACTER_WIZARD_FIRE, pos, (*iter).second, MESH_TYPE::CHARACTER);
			break;
		case ELEMENT_COLD:
			pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
				CHARACTER_WIZARD_COLD, pos, (*iter).second, MESH_TYPE::CHARACTER);
			break;
		case ELEMENT_DARKNESS:
			pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
				CHARACTER_WIZARD_DARKNESS, pos, (*iter).second, MESH_TYPE::CHARACTER);
			break;
		}
		Core::GetInstance()->CmdLstExecute();
		Core::GetInstance()->WaitForGpuComplete();
		m_pObjects[OBJ_PLAYER].push_back(pObj);
	}


#else
	pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		CHARACTER_WIZARD_COLD, MESH_TYPE::CHARACTER, XMFLOAT3(20.f, 0.f, 10.f));
	pPlayer = dynamic_cast<Player*>(pObj);
	m_pObjects[OBJ_PLAYER].push_back(pObj);
#endif


	Core::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pCamera = pCamera = dynamic_cast<Camera*>(pObj);
	//m_pCamera->SetMode(CAMERA_MODE::CAMERA_THIRD);
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
	//	XMFLOAT3(20.f, 2.f, 20.f));
	//m_pObjects[OBJ_SKILL].push_back(pObj);
	//
	//pObj = new FireShock(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	//m_pObjects[OBJ_SKILL].push_back(pObj);

	//pObj = new FireRing(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	//m_pObjects[OBJ_SKILL].push_back(pObj);
	//
	//pObj = new Bullet(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), "FireBall",
	//	XMFLOAT3(20.f, 1.f, 10.f), XMFLOAT3(0.f, 180.f, 0.f), 0.f);
	//m_pObjects[OBJ_BULLET].push_back(pObj);
	//
	//pObj = new IceBolt(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(20.f, 2.f, 15.f));
	//m_pObjects[OBJ_SKILL].push_back(pObj);
	//
	//pObj = new Meteor(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(20.f, 5.f, 15.f));
	//m_pObjects[OBJ_SKILL].push_back(pObj);



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

	pObj = new HpBar(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(105.f, 15.f, 512.f, 64.f), "HpBar");
	dynamic_cast<HpBar*>(pObj)->SetPlayer(pPlayer);
	m_pObjects[OBJ_UI].push_back(pObj);

	string name;
	if (pPlayer->GetNetworkInfo().CharacterType == WIZARD_FIRE)
		name = "Ui_Char_Fire";
	else if (pPlayer->GetNetworkInfo().CharacterType == WIZARD_COLD)
		name = "Ui_Char_Cold";
	else if (pPlayer->GetNetworkInfo().CharacterType == WIZARD_DARKNESS)
		name = "Ui_Char_Darkness";


	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(15.f, 15.f, 90.f, 90.f), name.c_str());
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(10.f, 10.f, 612.f, 100.f), "HpBarBase");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(WINCX - 400.f, 10.f, 300.f, 100.f), "Kill");
	m_pObjects[OBJ_UI].push_back(pObj);

	SkillController* pSkillCtrl = SkillController::GetInstance();
	pSkillCtrl->Initialize(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), this);

	ShopController* pShopCtrl = ShopController::GetInstance();
	pShopCtrl->Initialize(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), this);
	pShopCtrl->SetSkillCtrl(pSkillCtrl);
	pShopCtrl->SetRendering(true);

	m_pObjects[OBJ_UI].push_back(new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 20.f, WINCY * 0.5f - 20.f, 40.f, 40.f), "CrossHair"));

	m_pTimeTextCtrl = new TextController(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(WINCX / 2.f, 10.f, 50.f, 50.f), "00", this);
	m_pKillTextCtrl = new TextController(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(WINCX - 240.f, 10.f, 100.f, 100.f), "0", this);

	BuildInstanceCBs();
	BuildMaterialCBs();
}

void TestScene::LateInit()
{
	if (m_LateInit) return;

#ifdef NETWORK

	if (Network::GetInstance()->GetMyInfo().isRoom_Host)
		Network::GetInstance()->CallEvent(EVENT_ROUND_SHOPPING_START_REQUEST, 0);

#endif // NETWORK
	m_LateInit = true;
}
