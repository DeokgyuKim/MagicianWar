#pragma once
#include "framework.h"

class Core
{
private:
	Core() {}
	Core(const Core& rhs) {}
	~Core() {}
public:
	static Core*	GetInstance();
	static void		DestoryInstnace();
private:
	static Core* m_pInstance;
public:
	HRESULT InitDevice();
	void	Render_Begin();
	void	Render_End();
private:
	HRESULT CreateDevice();
	HRESULT CreateFence();
	HRESULT MsaaTest();
	HRESULT CreateCmdLstAndQueueAndAllocator();
	HRESULT CreateSwapchain();
	HRESULT CreateDescriptorHeap();
	HRESULT CreateRtvDsvBufferAndView();
	HRESULT SetViewportAndScissorrect();

public:
	void	CmdLstReset();
	void	CmdLstExecute();
	void	MoveToNextFrame();
	void	WaitForGpuComplete();

	ID3D12Device*				GetDevice() { return m_ptrDevice.Get(); }
	ID3D12GraphicsCommandList*	GetCmdLst() { return m_ptrCmdLst.Get(); }
private:
	ComPtr<IDXGIFactory4>				m_ptrDxgiFactory;
	ComPtr<IDXGISwapChain3>				m_ptrDxgiSwapChain;

	ComPtr<ID3D12Device>				m_ptrDevice;
	ComPtr<ID3D12Fence>					m_ptrFence;

	ComPtr<ID3D12CommandQueue>			m_ptrCmdQueue;
	ComPtr<ID3D12CommandAllocator>		m_ptrCmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_ptrCmdLst;

	ComPtr<ID3D12DescriptorHeap>		m_ptrRTVHeap;
	ComPtr<ID3D12DescriptorHeap>		m_ptrDSVHeap;

	ComPtr<ID3D12Resource>				m_ptrSwapChainBackBuffers[2];
	ComPtr<ID3D12Resource>				m_ptrDsv;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_SwapChainBackBufferRTVCPUHandles[2];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_DsvCPUHandles;

	D3D12_VIEWPORT						m_Viewport;
	D3D12_RECT							m_ScissorRect;

	UINT								m_iSwapChainBufferIndex;

	UINT64								m_nFenceValues[2];
	HANDLE								m_hFenceEvent;


	bool					m_bMsaa4xEnable;
	UINT					m_iMsaa4xQualityLevels;
};

