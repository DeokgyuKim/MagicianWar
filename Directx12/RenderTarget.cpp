#include "RenderTarget.h"
#include "Renderer.h"
#include "Core.h"

RenderTarget::RenderTarget(ID3D12Device* device, Renderer* pRenderer, const int width, const int height, D3D12_CLEAR_VALUE clear, DXGI_FORMAT format)
{
	m_pDevice = device;
	m_pRenderer = pRenderer;
	m_iWidth = width;
	m_iHeight = height;
	m_ClearValue = clear;
	Initialize(format);
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

void RenderTarget::SetShaderVariable(ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int RootParameterIdx)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += (m_SrvHandle.ptr - heap->GetCPUDescriptorHandleForHeapStart().ptr);
	cmdLst->SetGraphicsRootDescriptorTable(RootParameterIdx, handle);
}

void RenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* cmdLst)
{
	float clear[4] = { m_ClearValue.Color[0], m_ClearValue.Color[1], m_ClearValue.Color[2], m_ClearValue.Color[3] };
	cmdLst->ClearRenderTargetView(m_RtvHandle, clear, 0, NULL);
}

HRESULT RenderTarget::Initialize(DXGI_FORMAT format)
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
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	Core::GetInstance()->CmdLstReset();

	m_pDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &texDesc,
		D3D12_RESOURCE_STATE_COMMON, &m_ClearValue, IID_PPV_ARGS(m_ptrRenderTarget.GetAddressOf()));
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	m_SrvHandle = m_pRenderer->CreateShaderResourceView(m_ptrRenderTarget.Get(), srvDesc);
	m_UavHandle = m_pRenderer->CreateUnorderedAccessView(m_ptrRenderTarget.Get(), uavDesc);

	m_RtvHandle = Core::GetInstance()->CreateRenderTargetView(m_ptrRenderTarget.Get(), rtvDesc);

	return S_OK;
}
