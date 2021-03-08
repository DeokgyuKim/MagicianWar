#pragma once
#include "framework.h"

class RenderTarget;
class Renderer;
class RenderTargetMgr
{
private:
	RenderTargetMgr() {}
	RenderTargetMgr(const RenderTargetMgr& rhs) {}
	~RenderTargetMgr() {}
public:
	static RenderTargetMgr* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new RenderTargetMgr();
		return m_pInstance;
	}
	static void				DestroyInstance() {
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static RenderTargetMgr* m_pInstance;

public:
	void			BuildRenderTarget(ID3D12Device* device, Renderer* pRenderer);
	void			SetMultiRenderTarget(ID3D12GraphicsCommandList* cmdLst, string tagMRT);
	void			ClearMultiRenderTarget(ID3D12GraphicsCommandList* cmdLst, string tagMRT);

	RenderTarget*	GetRenderTarget(string tagRT);

private:
	unordered_map<string, RenderTarget*>		m_mapRenderTarget;
	unordered_map<string, vector<RenderTarget*>>	m_mapMRT;
};

