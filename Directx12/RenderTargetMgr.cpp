#include "RenderTargetMgr.h"
#include "RenderTarget.h"
#include "Core.h"

RenderTargetMgr* RenderTargetMgr::m_pInstance = NULL;

void RenderTargetMgr::BuildRenderTarget(ID3D12Device* device, Renderer* pRenderer)
{
	RenderTarget* Rt = NULL;

	D3D12_CLEAR_VALUE clear;
	clear.DepthStencil.Depth = 1.f;
	clear.DepthStencil.Stencil = 0.f;
	clear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	clear.Color[0] = 0.f;
	clear.Color[1] = 0.f;
	clear.Color[2] = 0.f;
	clear.Color[3] = 0.f;

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Diffuse"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Ambient"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Specular"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	clear.Color[3] = 0.f;

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Normal"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	clear.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_mapRenderTarget["Depth"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_mapRenderTarget["Position"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_mapRenderTarget["Emmissive"] = Rt;
	m_mapMRT["Deffered"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX * SHADOWRATIO, WINCY * SHADOWRATIO, clear, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_mapRenderTarget["LightDepth"] = Rt;
	m_mapMRT["Shadow"].push_back(Rt);
	clear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	clear.Color[0] = 0.f;
	clear.Color[1] = 0.f;
	clear.Color[2] = 0.f;
	clear.Color[3] = 1.f;

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Shade"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);

	clear.Color[0] = 0.f;
	clear.Color[1] = 0.f;
	clear.Color[2] = 0.f;
	clear.Color[3] = 1.f;
	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Specular"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);
	
	clear.Color[0] = 0.f;
	clear.Color[1] = 0.f;
	clear.Color[2] = 0.f;
	clear.Color[3] = 1.f;
	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["RimLight"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);
	
	clear.Color[0] = 0.f;
	clear.Color[1] = 0.f;
	clear.Color[2] = 0.f;
	clear.Color[3] = 1.f;
	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["OutLine"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["BloomAlpha"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);

	clear.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_mapRenderTarget["Bloom"] = Rt;
	m_mapMRT["Shade"].push_back(Rt);
	clear.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Blend"] = Rt;
	m_mapMRT["Blend"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Distortion"] = Rt;
	m_mapMRT["Blend"].push_back(Rt);

	Rt = new RenderTarget(device, pRenderer, WINCX, WINCY, clear);
	m_mapRenderTarget["Post"] = Rt;
	m_mapMRT["PostProcess"].push_back(Rt);
}

void RenderTargetMgr::SetMultiRenderTarget(ID3D12GraphicsCommandList* cmdLst, string tagMRT, D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle)
{
	auto iter = m_mapMRT.find(tagMRT);
	if (iter == m_mapMRT.end())
	{
		cout << "Error : No Exist Mrt Tag In RenderTargetMgr!" << endl;
		return;
	}

	for (int i = 0; i < (*iter).second.size(); ++i)
		(*iter).second[i]->ResourceBarrier(cmdLst, D3D12_RESOURCE_STATE_RENDER_TARGET);

	Core::GetInstance()->SetRenderTarget((*iter).second.size(), (*iter).second.front()->GetRtvHandle(), DsvHandle);
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
		(*iter).second[i]->ClearRenderTarget(cmdLst);
		//cmdLst->ClearRenderTargetView((*iter).second[i]->GetRtvHandle(), pfClearColor, 0, NULL);
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
