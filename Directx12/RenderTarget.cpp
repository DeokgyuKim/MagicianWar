#include "RenderTarget.h"
#include "Renderer.h"
#include "Core.h"

RenderTarget::RenderTarget(ID3D12Device* device, Renderer* pRenderer, const int width, const int height)
{
	m_pDevice = device;
	m_pRenderer = pRenderer;
	m_iWidth = width;
	m_iHeight = height;
	Initialize();
	m_CurState = D3D12_RESOURCE_STATE_COMMON;
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::ResourceBarrier(ID3D12GraphicsCommandList* cmdLst, D3D12_RESOURCE_STATES postState)
{
	if (m_CurState == postState)
		return;

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ptrRenderTarget.Get();
	d3dResourceBarrier.Transition.StateBefore = m_CurState;
	d3dResourceBarrier.Transition.StateAfter = postState;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdLst->ResourceBarrier(1, &d3dResourceBarrier);

	m_CurState = postState;
}

HRESULT RenderTarget::Initialize()
{
	D3D12_HEAP_PROPERTIES properties;
	properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	properties.CreationNodeMask = 1;
	properties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = m_iWidth;
	texDesc.Height = m_iHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;


	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.125f;
	clearValue.Color[2] = 0.3f;
	clearValue.Color[3] = 1.0f;
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	Core::GetInstance()->CmdLstReset();

	m_pDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &texDesc,
		D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(m_ptrRenderTarget.GetAddressOf()));
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	m_SrvHandle = m_pRenderer->CreateShaderResourceView(m_ptrRenderTarget.Get(), srvDesc);
	m_UavHandle = m_pRenderer->CreateUnorderedAccessView(m_ptrRenderTarget.Get(), uavDesc);

	m_RtvHandle = Core::GetInstance()->CreateRenderTargetView(m_ptrRenderTarget.Get(), rtvDesc);

	return S_OK;
}
