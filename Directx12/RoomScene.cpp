#include "RoomScene.h"
#include "TestScene.h"

#include "Network.h"
#include "MainApp.h"

#include "Core.h"
#include "Camera.h"
#include "Renderer.h"
#include "UI.h"
#include "Button.h"
#include "RadioButton.h"

#include "CharInfoController.h"

void readyroom();
void unreadyroom();
void exitroom();
void selectredteam();
void selectblueteam();

RoomScene::RoomScene()
{
	Initialize();
	m_RoomPlayer[0].host = true;
	m_RoomPlayer[0].chartype = WIZARD_FIRE;
	m_RoomPlayer[0].readystate = false;
	m_RoomPlayer[0].used = true;
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
	m_eSceneType = ROOM;
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

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, WINCX, WINCY), "RoomScene");
	m_pObjects[OBJ_UI].push_back(pObj);


	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 200.f, 700.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTextTextureName("Ui_Text_Ready");
	m_pButton->SetTag(BUTTON_GAME_READY);
	m_pButton->SetEventButtonOn(readyroom);
	m_pButton->SetEventButtonOff(unreadyroom);

	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f - 200.f, 860.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTextTextureName("Ui_Text_Exit");
	m_pButton->SetTag(BUTTON_ROOM_EXIT);
	m_pButton->SetEventButtonOn(exitroom);


	///Radio
	pObj = m_pRadio[0] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(685.f, 370.f, 150.f, 150.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadio[0]->SetTag(WIZARD_FIRE);

	pObj = m_pRadio[1] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(885.f, 370.f, 150.f, 150.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadio[1]->SetTag(WIZARD_COLD);

	pObj = m_pRadio[2] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(1085.f, 370.f, 150.f, 150.f), "Ui_Char_Base", "Ui_Char_Mouse", "Ui_Char_Click");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadio[2]->SetTag(WIZARD_DARKNESS);

	m_pRadio[0]->SetOthers(m_pRadio[1]); m_pRadio[0]->SetOthers(m_pRadio[2]);
	m_pRadio[1]->SetOthers(m_pRadio[0]); m_pRadio[1]->SetOthers(m_pRadio[2]);
	m_pRadio[2]->SetOthers(m_pRadio[0]); m_pRadio[2]->SetOthers(m_pRadio[1]);

	m_pRadio[0]->SetButtonState(BUTTON_STATE::ON);
	//RadioImage
	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(690.f, 375.f, 140.f, 140.f), "Ui_Char_Fire");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(890.f, 375.f, 140.f, 140.f), "Ui_Char_Cold");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(1090.f, 375.f, 140.f, 140.f), "Ui_Char_Darkness");
	m_pObjects[OBJ_UI].push_back(pObj);

	pObj = m_pRadioTeam[0] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(0.f, 100.f, 512.f, 180.f), "Ui_Text_RedTeam", "Ui_Text_RedTeam", "Ui_Text_RedTeam");
	m_pObjects[OBJ_UI].push_back(pObj);
	m_pRadioTeam[0]->SetTag(BUTTON_TAG::BUTTON_ROOM_END);
	m_pRadioTeam[0]->SetEventButtonOn(selectredteam);


	pObj = m_pRadioTeam[1] = new RadioButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(1408.f, 100.f, 512.f, 180.f), "Ui_Text_BlueTeam", "Ui_Text_BlueTeam", "Ui_Text_BlueTeam");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pRadioTeam[1]->SetTag(BUTTON_TAG::BUTTON_ROOM_END + 1);
	m_pRadioTeam[0]->SetOthers(m_pRadioTeam[1]);
	m_pRadioTeam[1]->SetOthers(m_pRadioTeam[0]);
	m_pRadioTeam[0]->SetEventButtonOn(selectblueteam);



	m_pCharInfoCtrl = new CharInfoController(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), this);
	for (int i = 0; i < 8; ++i)
		m_pCharInfoCtrl->AddPlayer(i, &m_RoomPlayer[i]);
}

void readyroom()
{
	Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_READY_ON, 0);
}
void unreadyroom()
{
	Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_READY_OFF, 0);
}
void exitroom()
{
	Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_EXIT, 0);
}
void selectredteam()
{
	Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_SELECT_TEAM, 1, TEAM_RED);
}
void selectblueteam()
{
	Network::GetInstance()->CallEvent(EVENT_ROOM_PLAYER_SELECT_TEAM, 1, TEAM_BLUE);
}