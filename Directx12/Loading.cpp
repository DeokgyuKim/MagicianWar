#include "Loading.h"
#include "Core.h"
#include "TextureMgr.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"
#include "StaticMeshMgr.h"
#include "StaticObject.h"
#include "Renderer.h"

Loading::Loading(Core* core, ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	m_pCore = core;
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pHeap = heap;
	Initialize();
}

void Loading::Initialize()
{
	InitializeCriticalSection(&m_Crt);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadMain, this, 0, NULL);
}

void Loading::LoadingTextures()
{
	//m_pCore->CmdLstForLoadingReset();
	TextureMgr::GetInstance()->BuildTextures(m_pDevice, m_pCmdLst, m_pHeap, this);
}

void Loading::LoadingModels()
{
	MeshMgr::GetInstnace()->BuildModels(this);
}

void Loading::LoadingSkinnedModels()
{
	MeshMgr::GetInstnace()->BuildSkinnedModels(this);
}

void Loading::LoadingAnimations()
{
	AnimationMgr::GetInstance()->BuildAnimations(this);
	m_pCore->CmdLstForLoadingClose();
	m_bFinish = true;
}

void Loading::Release()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);
}

unsigned int Loading::ThreadMain(void* pArg)
{
	Loading* pLoading = (Loading*)pArg;

	EnterCriticalSection(pLoading->GetCrt());

	pLoading->LoadingTextures();
	pLoading->LoadingModels();
	pLoading->LoadingSkinnedModels();
	pLoading->LoadingAnimations();

	StaticMeshMgr::GetInstance()->LoadMeshInfo("../Data/Map1Data.txt");
	//StaticMeshMgr::GetInstance()->LoadMeshInfo("../Data/Saved.txt");
	multimap<string*, TransformStruct> mmapInfo = StaticMeshMgr::GetInstance()->GetMapMeshInfo();
	Object* pObj = NULL;
	for (auto iter = mmapInfo.begin(); iter != mmapInfo.end(); ++iter)
	{
		pObj = new StaticObject(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
			(*iter).second.xmfPosition, (*iter).second.xmfRotate, (*iter).second.xmfScale, *(*iter).first, true);
		delete pObj;
	}
	CPhysXMgr::GetInstance()->CreatePlaneTriangleStaticMesh();

	LeaveCriticalSection(pLoading->GetCrt());
	_endthreadex(0);

	return 0;
}
