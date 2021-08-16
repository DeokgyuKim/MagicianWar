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
#include "ServerButton.h"
#include "Bar.h"
#include "RadioButton.h"
#include "RoomRadio.h"
#include "RoomRadioController.h"
#include "TextController.h"
#include "Cursor.h"

#include "StaticMeshMgr.h"
#include "TextureMgr.h"

#include "Loading.h"
#include "SoundMgr.h"

void makeroom();
void joinroom();

LobbyScene::LobbyScene(bool bRetry)
{
	Initialize(bRetry);
}

int LobbyScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);

#ifndef NETWORK
	if (m_pButton->GetButtonState() == BUTTON_STATE::ON)
	{
		Network::GetInstance()->CallEvent(EVENT_SCENE_CHANGE, 1, STAGE_SCENE);
		return -1;
	}
#endif // NETWORK

	if (GetAsyncKeyState(VK_NUMPAD0) & 0x0001)
		m_rbController->AddRoom(this, iTag++);

	if (GetAsyncKeyState(VK_NUMPAD1) & 0x0001)
		m_rbController->RemoveRoom(this, --iTag);

	vector<int> idx;
	for (auto iter : *Network::GetInstance()->GetRoomInfo())
	{
		auto clientiter = m_mapClientRooms.find(iter.first);
		if (clientiter == m_mapClientRooms.end())
		{
			m_rbController->AddRoom(this, iter.second.room_num);
			idx.push_back(iter.second.room_num);
		}
	}

	for (int index : idx)
	{
		m_mapClientRooms[index] = Network::GetInstance()->GetRoomInfo()->find(index)->second;
	}
	idx.clear();
	
	for (auto iter : m_mapClientRooms)
	{
		auto serveriter = Network::GetInstance()->GetRoomInfo()->find(iter.first);
		if (serveriter == Network::GetInstance()->GetRoomInfo()->end())
		{
			m_rbController->RemoveRoom(this, iter.first);
			idx.push_back(iter.first);
		}
	}
	for (int index : idx)
	{
		m_mapClientRooms.erase(index);
	}
	idx.clear();


	return 0;
}

void LobbyScene::Initialize(bool bRetry)
{

	m_eSceneType = LOBBY;
	Object* pObj = NULL;
	Player* pPlayer = NULL;
	Camera* pCamera = NULL;


	//::GetInstance()->CmdLstReset();
	pObj = new Camera(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pCamera = pCamera = dynamic_cast<Camera*>(pObj);
	Renderer::GetInstance()->SetCamera(dynamic_cast<Camera*>(pObj));
	m_pObjects[OBJ_CAMERA].push_back(pObj);
	//Core::GetInstance()->CmdLstExecute();
	//Core::GetInstance()->WaitForGpuComplete();

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(0.f, 0.f, WINCX, WINCY), "Lobby");
	m_pObjects[OBJ_UI].push_back(pObj);

#ifdef NETWORK
	//pObj = new ServerButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(WINCX * 0.5f + 400.f, 150.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	//dynamic_cast<ServerButton*>(pObj)->SetTag(BUTTON_ROOM_MAKE, EVENT_LOBBY_ROOM_MAKE_REQUEST);
	//m_pObjects[OBJ_UI].push_back(pObj);
	//
	//
	//pObj = new ServerButton(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
	//	XMFLOAT4(WINCX * 0.5f + 400.f, 350.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	//dynamic_cast<ServerButton*>(pObj)->SetTag(BUTTON_ROOM_JOIN, EVENT_LOBBY_ROOM_JOIN_REQUEST);
	//m_pObjects[OBJ_UI].push_back(pObj);
	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 650.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);


	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(BUTTON_ROOM_MAKE);
	m_pButton->SetTextTextureName("Ui_Text_MakeRoom");
	m_pButton->SetEventButtonOn(makeroom);

	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 850.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(BUTTON_ROOM_JOIN);
	m_pButton->SetTextTextureName("Ui_Text_JoinRoom");
	m_pButton->SetEventButtonOn(joinroom);

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(150.f, 250.f, 1080.f, 800.f), "RoomBase");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_rbController = new RoomRadioController(XMFLOAT4(150.f, 250.f, 1080.f, 800.f));
	

#else
	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 650.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);


	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(BUTTON_ROOM_MAKE);
	m_pButton->SetTextTextureName("Ui_Text_MakeRoom");

	pObj = new Button(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(WINCX * 0.5f + 400.f, 850.f, 400.f, 130.f), "ButtonBase", "ButtonMouseOn", "ButtonOn");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_pButton = dynamic_cast<Button*>(pObj);
	m_pButton->SetTag(BUTTON_ROOM_JOIN);
	m_pButton->SetTextTextureName("Ui_Text_JoinRoom");

	pObj = new UI(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), XMFLOAT4(150.f, 250.f, 1080.f, 800.f), "RoomBase");
	m_pObjects[OBJ_UI].push_back(pObj);

	m_rbController = new RoomRadioController(XMFLOAT4(150.f, 250.f, 1080.f, 800.f));
	m_rbController->AddRoom(this, iTag++);
	m_rbController->AddRoom(this, iTag++);

#endif // !NETWORK

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
	m_pObjects[OBJ_UI].push_back(new Cursor(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()));


#ifdef NETWORK
	if (!bRetry) {
		Network::GetInstance()->Init(Network::GetInstance()->LoadServerIPtxt("../Data/ServerIP.txt"));
	}
	Network::GetInstance()->Lobby_Init();
#endif

	//ShowCursor(TRUE);
}

void makeroom()
{
	Network::GetInstance()->CallEvent(EVENT_LOBBY_ROOM_MAKE_REQUEST, 0);
}
void joinroom()
{
	Network::GetInstance()->CallEvent(EVENT_LOBBY_ROOM_JOIN_REQUEST, 0);
}