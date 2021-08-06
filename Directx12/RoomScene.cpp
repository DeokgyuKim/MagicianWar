#include "RoomScene.h"
#include "TestScene.h"

#include "Network.h"
#include "MainApp.h"

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "UI.h"
#include "Button.h"
#include "ServerButton.h"
#include "RadioButton.h"

RoomScene::RoomScene()
{
    Initialize();
}

int RoomScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);

#ifdef NETWORK
	//if (Network::GetInstance()->GetCurScene() == STAGE_Scene)
	//{
	//	Network::GetInstance()->CallEvent(EVENT_SCENE_CHANGE, 1, STAGE_SCENE);
	//	return -1;
	//}
#else
	if (m_pButton->GetButtonState() == BUTTON_STATE::ON)
	{
		TestScene* Scene = new TestScene();
		MainApp::GetInstance()->ChangeScene(Scene);
		return -1;
	}
#endif // NETWORK



	return 0;
}

void RoomScene::Initialize()
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


	pObj = new ServerButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 200.f, 850.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	dynamic_cast<ServerButton*>(pObj)->SetTag(BUTTON_GAME_READY,EVENT_ROOM_PLAYER_READY_ON);
	m_pObjects[OBJ_UI].push_back(pObj);


	pObj = new ServerButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 200.f, 150.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	dynamic_cast<ServerButton*>(pObj)->SetTag(BUTTON_ROOM_EXIT, EVENT_ROOM_PLAYER_EXIT);
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new ServerButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 800.f, 50.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	dynamic_cast<ServerButton*>(pObj)->SetTag(BUTTON_BLUETEAM, EVENT_ROOM_PLAYER_SELECT_TEAM);
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new ServerButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 50.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	dynamic_cast<ServerButton*>(pObj)->SetTag(BUTTON_REDTEAM, EVENT_ROOM_PLAYER_SELECT_TEAM);
	m_pObjects[OBJ_UI].push_back(pObj);


	///Radio
	pObj = m_pRadio[0] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(560.f, 500.f, 200.f, 200.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadio[0]->SetTag(WIZARD_FIRE);

	pObj = m_pRadio[1] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(860.f, 500.f, 200.f, 200.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadio[1]->SetTag(WIZARD_COLD);

	pObj = m_pRadio[2] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(1160.f, 500.f, 200.f, 200.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadio[2]->SetTag(WIZARD_DARKNESS);

	m_pRadio[0]->SetOthers(m_pRadio[1]); m_pRadio[0]->SetOthers(m_pRadio[2]);
	m_pRadio[1]->SetOthers(m_pRadio[0]); m_pRadio[1]->SetOthers(m_pRadio[2]);
	m_pRadio[2]->SetOthers(m_pRadio[0]); m_pRadio[2]->SetOthers(m_pRadio[1]);

	m_pRadio[0]->SetButtonState(BUTTON_STATE::ON);

	//RadioImage
	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(570.f, 510.f, 180.f, 180.f), "Ui_Char_Fire");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(870.f, 510.f, 180.f, 180.f), "Ui_Char_Cold");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(1170.f, 510.f, 180.f, 180.f), "Ui_Char_Darkness");
	m_pObjects[OBJ_UI].push_back(pObj);

#ifdef NETWORK
	Network::GetInstance()->Room_Init();
#endif // NETWORK


}
