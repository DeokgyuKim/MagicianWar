#pragma once
#include "framework.h"

class Core;
class Loading
{
public:
	Loading(Core* core, ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	~Loading() { Release(); }
private:
	void Initialize();
	void LoadingTextures();
	void LoadingModels();
	void LoadingSkinnedModels();
	void LoadingAnimations();
	void Release();
public:
	CRITICAL_SECTION*	GetCrt() { return &m_Crt; }
	bool GetFinish() { return m_bFinish; }
	void AddCount() { 
		++m_iCnt; 
#ifdef _DEBUG
#else
		Sleep(10); 
#endif // _DEBUG

	}
	int GetCount() { return m_iCnt; }

	static unsigned int APIENTRY ThreadMain(void* pArg);
private:
	Core*						m_pCore;
	ID3D12Device*				m_pDevice;
	ID3D12GraphicsCommandList*	m_pCmdLst;
	ID3D12DescriptorHeap*		m_pHeap;

	HANDLE m_hThread;
	CRITICAL_SECTION m_Crt;
	bool m_bFinish;

	int m_iCnt = 0;
};

