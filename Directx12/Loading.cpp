#include "Loading.h"
#include "Core.h"
#include "TextureMgr.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"

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
	TextureMgr::GetInstance()->BuildTextures(m_pDevice, m_pCmdLst, m_pHeap);
}

void Loading::LoadingModels()
{
	MeshMgr::GetInstnace()->BuildModels();
}

void Loading::LoadingSkinnedModels()
{
	MeshMgr::GetInstnace()->BuildSkinnedModels();
}

void Loading::LoadingAnimations()
{
	AnimationMgr::GetInstance()->BuildAnimations();
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

	LeaveCriticalSection(pLoading->GetCrt());
	_endthreadex(0);

	return 0;
}
