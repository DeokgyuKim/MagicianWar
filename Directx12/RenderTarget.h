#pragma once
#include "framework.h"

class Renderer;

class RenderTarget
{
public:
	RenderTarget(ID3D12Device* device, Renderer* pRenderer, const int width, const int height, D3D12_CLEAR_VALUE clear, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();
public:
	void	ResourceBarrier(ID3D12GraphicsCommandList* cmdLst, D3D12_RESOURCE_STATES postState);
	void	SetShaderVariable(ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int RootParameterIdx);
	void	ClearRenderTarget(ID3D12GraphicsCommandList* cmdLst);
public:
	D3D12_CPU_DESCRIPTOR_HANDLE	GetSrvHandle() { return m_SrvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle() { return m_RtvHandle; }
	ID3D12Resource*				GetResource() { return m_ptrRenderTarget.Get(); }
private:
	HRESULT	Initialize(DXGI_FORMAT format);
private:
	ID3D12Device*			m_pDevice;
	ComPtr<ID3D12Resource>	m_ptrRenderTarget;

	Renderer*				m_pRenderer = NULL;

	int	m_iWidth, m_iHeight;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_SrvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_UavHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_RtvHandle;
	D3D12_RESOURCE_STATES		m_CurState;

	D3D12_CLEAR_VALUE			m_ClearValue;
};

