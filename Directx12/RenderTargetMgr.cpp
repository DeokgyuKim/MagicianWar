#include "RenderTargetMgr.h"
#include "RenderTarget.h"
#include "Core.h"

RenderTargetMgr* RenderTargetMgr::m_pInstance = NULL;

void RenderTargetMgr::BuildRenderTarget(ID3D12Device* device, Renderer* pRenderer)
{
	RenderTarget* Rt = NULL;

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY);
	m_mapRenderTarget["Diffuse"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY);
	m_mapRenderTarget["Ambient"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY);
	m_mapRenderTarget["Specular"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY);
	m_mapRenderTarget["Normal"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY);
	m_mapRenderTarget["Depth"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY);
	m_mapRenderTarget["Shade"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);
}

void RenderTargetMgr::SetMultiRenderTarget(ID3D12GraphicsCommandList* cmdLst, string tagMRT)
{
	auto iter = m_mapMRT.find(tagMRT);
	if (iter == m_mapMRT.end())
	{
		cout << "Error : No Exist Mrt Tag In RenderTargetMgr!" << endl;
		return;
	}

	for (int i = 0; i < (*iter).second.size(); ++i)
		(*iter).second[i]->ResourceBarrier(cmdLst, D3D12_RESOURCE_STATE_RENDER_TARGET);

	Core::GetInstance()->SetRenderTarget((*iter).second.size(), (*iter).second.front()->GetRtvHandle());
}

void RenderTargetMgr::ClearMultiRenderTarget(ID3D12GraphicsCommandList* cmdLst, string tagMRT)
{
	auto iter = m_mapMRT.find(tagMRT);
	if (iter == m_mapMRT.end())
	{
		cout << "Error : No Exist Mrt Tag In RenderTargetMgr!" << endl;
		return;
	}

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	for (int i = 0; i < (*iter).second.size(); ++i)
	{
		(*iter).second[i]->ResourceBarrier(cmdLst, D3D12_RESOURCE_STATE_RENDER_TARGET);
		cmdLst->ClearRenderTargetView((*iter).second[i]->GetRtvHandle(), pfClearColor, 0, NULL);
	}

}

RenderTarget* RenderTargetMgr::GetRenderTarget(string tagRT)
{
	auto iter = m_mapRenderTarget.find(tagRT);
	if (iter == m_mapRenderTarget.end())
	{
		cout << "Error : No Exist RenderTarget Tag In RenderTargetMgr!" << endl;
		return nullptr;
	}
	return iter->second;
}
