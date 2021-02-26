#include "TestScene.h"
#include "Triangle.h"
#include "Plane.h"
#include "Core.h"

TestScene::TestScene()
{
	Initialize();
}

void TestScene::Initialize()
{
	Object* pObj = NULL;

	//Core::GetInstance()->CmdLstReset();
	//pObj = new Triangle(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst());
	//Core::GetInstance()->CmdLstExecute();
	//Core::GetInstance()->WaitForGpuComplete();
	//
	//m_pObjects[OBJ_PLAYER].push_back(pObj);

	Core::GetInstance()->CmdLstReset();
	pObj = new Plane(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst());
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_pObjects[OBJ_PLAYER].push_back(pObj);
}
