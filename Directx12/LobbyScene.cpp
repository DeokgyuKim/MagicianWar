#include "LobbyScene.h"
#include "RoomScene.h"

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
#include "RadioButton.h"

#include "StaticMeshMgr.h"
#include "TextureMgr.h"

#include "Loading.h"

LobbyScene::LobbyScene(bool bRetry)
{
	Initialize(bRetry);
}

int LobbyScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);

#ifdef NETWORK
	//if (Network::GetInstance()->GetCurScene() == STAGE_Scene)
	//{
	//	TestScene* Scene = new TestScene();
	//	MainApp::GetInstance()->ChangeScene(Scene);
	//	return -1;
	//}
#else
	if (m_pButton->GetButtonState() == BUTTON_STATE::ON)
	{
		Network::GetInstance()->CallEvent(EVENT_SCENE_CHANGE, 1, STAGE_SCENE);
		return -1;
	}
#endif // NETWORK
	


	return 0;
}

void LobbyScene::Initialize(bool bRetry)
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
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, WINCX, WINCY), "Lobby");
	m_pObjects[OBJ_UI].push_back(pObj);


	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 150.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(BUTTON_ROOM_MAKE);

	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 350.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(BUTTON_ROOM_JOIN);

	/////Radio
	//pObj = m_pRadio[0] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(560.f, 500.f, 200.f, 200.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	//m_pObjects[OBJ_UI].push_back(pObj);
	//m_pRadio[0]->SetTag(WIZARD_FIRE);

	//pObj = m_pRadio[1] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(860.f, 500.f, 200.f, 200.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	//m_pObjects[OBJ_UI].push_back(pObj);
	//m_pRadio[1]->SetTag(WIZARD_COLD);

	//pObj = m_pRadio[2] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(1160.f, 500.f, 200.f, 200.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	//m_pObjects[OBJ_UI].push_back(pObj);
	//m_pRadio[2]->SetTag(WIZARD_DARKNESS);

	//m_pRadio[0]->SetOthers(m_pRadio[1]); m_pRadio[0]->SetOthers(m_pRadio[2]);
	//m_pRadio[1]->SetOthers(m_pRadio[0]); m_pRadio[1]->SetOthers(m_pRadio[2]);
	//m_pRadio[2]->SetOthers(m_pRadio[0]); m_pRadio[2]->SetOthers(m_pRadio[1]);

	//m_pRadio[0]->SetButtonState(BUTTON_STATE::ON);

	////RadioImage
	//pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(570.f, 510.f, 180.f, 180.f), "Ui_Char_Fire");
	//m_pObjects[OBJ_UI].push_back(pObj);

	//pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(870.f, 510.f, 180.f, 180.f), "Ui_Char_Cold");
	//m_pObjects[OBJ_UI].push_back(pObj);

	//pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(1170.f, 510.f, 180.f, 180.f), "Ui_Char_Darkness");
	//m_pObjects[OBJ_UI].push_back(pObj);


#ifdef NETWORK
	if(!bRetry)
		Network::GetInstance()->Init(Network::GetInstance()->LoadServerIPtxt("../Data/ServerIP.txt"));
#endif

	ShowCursor(TRUE);
}

