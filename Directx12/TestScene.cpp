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

#include "Network.h"

TestScene::TestScene()
{
	Initialize();
}

int TestScene::Update(const float& fTimeDelta)
{
	Scene::Update(fTimeDelta);

	if (GetAsyncKeyState(VK_F4) & 0x0001)
	{
		ToolMode = !ToolMode;
	}

	POINT pt;
	if (ToolMode)
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
		{
			GetCursorPos(&pt);
			ScreenToClient(g_hWnd, &pt);

			if (pt.x > WINCX || pt.x < 0 || pt.y > WINCY || pt.y < 0)
				return 0;

			cout << "pt: " << pt.x << ", " << pt.y << "\tPos: ";
			
			XMFLOAT3 MousePos;
			MousePos.x = pt.x / (WINCX * 0.5f) - 1.f;
			MousePos.y = pt.y / -(WINCY * 0.5f) + 1.f;
			MousePos.z = 0.f;

			XMFLOAT4X4 proj, view;

			proj = m_pCamera->GetProjMatrix();
			view = m_pCamera->GetViewMatrix();

			XMStoreFloat3(&MousePos, XMVector3TransformCoord(XMLoadFloat3(&MousePos), XMMatrixInverse(NULL, XMLoadFloat4x4(&proj))));

			XMFLOAT3 rayPos, rayDir, rayPos2;
			rayPos = XMFLOAT3(0.f, 0.f, 0.f);
			XMStoreFloat3(&rayDir, XMLoadFloat3(&MousePos) - XMLoadFloat3(&rayPos));

			XMStoreFloat3(&rayPos, XMVector3TransformCoord(XMLoadFloat3(&rayPos), XMMatrixInverse(NULL, XMLoadFloat4x4(&view))));
			XMStoreFloat3(&rayDir, XMVector3TransformCoord(XMLoadFloat3(&rayDir), XMMatrixInverse(NULL, XMLoadFloat4x4(&view))));

			XMFLOAT3 point[3] = {
				XMFLOAT3(-0.5f,	0.f, +0.5f),
				XMFLOAT3(+0.5f,	0.f, +0.5f),
				XMFLOAT3(+0.5f,	0.f, -0.5f)
			};

			rayPos2.x = rayPos.x + rayDir.x;
			rayPos2.y = rayPos.y + rayDir.y;
			rayPos2.z = rayPos.z + rayDir.z;

			XMVECTOR plane = XMPlaneFromPoints(XMLoadFloat3(&point[0]), XMLoadFloat3(&point[1]), XMLoadFloat3(&point[2]));
			XMVECTOR pos = XMPlaneIntersectLine(plane, XMLoadFloat3(&rayPos), XMLoadFloat3(&(rayDir)));
			XMFLOAT3 position;
			XMStoreFloat3(&position, pos);

			cout << position.x << ", " << position.y << ", " << position.z << endl;

			Object* pObj;

			//pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(), position, HOUSE_02);
			//(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());

			m_pObjects[OBJ_STATIC].push_back(pObj);
		}
	}

	return 0;
}

void TestScene::LateUpdate(const float& fTimeDelta)
{
	Scene::LateUpdate(fTimeDelta);

#ifdef NETWORK
	Object* pObj = GetPlayer();
	if (pObj != NULL)
		Network::GetInstance()->SetMyPlayerInfo(dynamic_cast<Player*>(pObj));
#else
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
		CHARACTER_WIZARD_01, pos);
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
			CHARACTER_WIZARD_01, pos);
		Core::GetInstance()->CmdLstExecute();
		Core::GetInstance()->WaitForGpuComplete();
		m_pObjects[OBJ_PLAYER].push_back(pObj);
	}


#else
	pObj = new Player(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		CHARACTER_WIZARD_01);
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

	pObj = new FireRing(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	m_pObjects[OBJ_SKILL].push_back(pObj);

	//Core::GetInstance()->CmdLstReset();
	//pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance());
	////(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance()->GetHeap());
	//Core::GetInstance()->CmdLstExecute();
	//Core::GetInstance()->WaitForGpuComplete();
	//
	//m_pObjects[OBJ_STATIC].push_back(pObj);
	
	
	BuildInstanceCBs();
	BuildMaterialCBs();
}
