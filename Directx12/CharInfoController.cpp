#include "CharInfoController.h"
#include "Scene.h"

CharInfoController::CharInfoController(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene)
{
	Initialize(device, cmdLst, pRenderer, pScene);
}

CharInfoController::~CharInfoController()
{
}

void CharInfoController::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, Scene* pScene)
{
	for (int i = 0; i < 4; ++i)
	{
		CharInfo* pInfo = new CharInfo(device, cmdLst, pRenderer, XMFLOAT4(100.f, 270.f + 200.f * i, 512.f, 140.f), "Char");
		pScene->PushObject(pInfo, OBJ_TYPE::OBJ_UI);
		pInfo->PushPortraitInScene(pScene);
		m_vecCharInfos.push_back(pInfo);
	}
	for (int i = 0; i < 4; ++i)
	{
		CharInfo* pInfo = new CharInfo(device, cmdLst, pRenderer, XMFLOAT4(1308.f, 270.f + 200.f * i, 512.f, 140.f), "Char");
		pScene->PushObject(pInfo, OBJ_TYPE::OBJ_UI);
		pInfo->PushPortraitInScene(pScene);
		m_vecCharInfos.push_back(pInfo);
	}
}

void CharInfoController::AddPlayer(int idx, Ui_Roomplayer* pPlayer)
{
	m_vecCharInfos[idx]->SetRoomPlayer(pPlayer);
}
