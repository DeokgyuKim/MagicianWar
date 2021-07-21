#include "Core.h"
#include "Renderer.h"
#include "MeshMgr.h"
#include "AnimationMgr.h"
#include "RenderTarget.h"

Core* Core::m_pInstance = NULL;

Core* Core::GetInstance()
{
    if (m_pInstance == NULL)
        m_pInstance = new Core;
    return m_pInstance;
}

void Core::DestoryInstnace()
{
    if (m_pInstance != NULL)
        delete m_pInstance;
    m_pInstance = NULL;
}

HRESULT Core::InitDevice()
{
    if (FAILED(CreateDevice())) return E_FAIL;
    if (FAILED(CreateFence())) return E_FAIL;
    if (FAILED(MsaaTest())) return E_FAIL;
    if (FAILED(CreateCmdLstAndQueueAndAllocator())) return E_FAIL;
    if (FAILED(CreateSwapchain())) return E_FAIL;
    if (FAILED(CreateDescriptorHeap())) return E_FAIL;
    if (FAILED(CreateRtvDsvBufferAndView())) return E_FAIL;
    if (FAILED(SetViewportAndScissorrect())) return E_FAIL;

    Renderer::GetInstance()->InitRenderer(this, m_ptrDevice.Get(), m_ptrCmdLst.Get());
    MeshMgr::GetInstnace()->InitMeshMgr(this, m_ptrDevice.Get(), m_ptrCmdLstForLoading.Get());
    AnimationMgr::GetInstance()->InitAnimationMgr(this, m_ptrDevice.Get(), m_ptrCmdLstForLoading.Get());
    return S_OK;
}



void Core::Render_Begin()
{
    m_ptrCmdAllocator->Reset();
    m_ptrCmdLst->Reset(m_ptrCmdAllocator.Get(), NULL);

    m_ptrCmdLst->RSSetViewports(1, &m_Viewport);
    m_ptrCmdLst->RSSetScissorRects(1, &m_ScissorRect);

    D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_ptrRTVHeap->GetCPUDescriptorHandleForHeapStart();
    d3dRtvCPUDescriptorHandle.ptr += (m_iSwapChainBufferIndex * ::gnRtvDescriptorIncrementSize);

    float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };

    D3D12_RESOURCE_BARRIER d3dResourceBarrier;
    d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier.Transition.pResource = m_ptrSwapChainBackBuffers[m_iSwapChainBufferIndex].Get();
    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_ptrCmdLst->ResourceBarrier(1, &d3dResourceBarrier);

    m_ptrCmdLst->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, NULL);
    m_ptrCmdLst->ClearDepthStencilView(m_DsvCPUHandles, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
    m_ptrCmdLst->ClearDepthStencilView(m_DsvForShadeCPUHandles, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
    m_ptrCmdLst->ClearDepthStencilView(m_DsvForShadowCPUHandles, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
    m_ptrCmdLst->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &m_DsvCPUHandles);

}

void Core::Render_End()
{
    D3D12_RESOURCE_BARRIER d3dResourceBarrier;
    d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier.Transition.pResource = m_ptrSwapChainBackBuffers[m_iSwapChainBufferIndex].Get();
    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_ptrCmdLst->ResourceBarrier(1, &d3dResourceBarrier);
    
    m_ptrCmdLst->Close();
    ID3D12CommandList* ppd3dCommandLists[] = { m_ptrCmdLst.Get() };
    m_ptrCmdQueue->ExecuteCommandLists(1, ppd3dCommandLists);
    
    m_ptrDxgiSwapChain->Present(0, 0);

    WaitForGpuComplete();
    MoveToNextFrame();
}

void Core::Render_EndTest(RenderTarget* pRt)
{
    D3D12_RESOURCE_BARRIER d3dResourceBarrier;
    d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier.Transition.pResource = m_ptrSwapChainBackBuffers[m_iSwapChainBufferIndex].Get();
    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_ptrCmdLst->ResourceBarrier(1, &d3dResourceBarrier);

    pRt->ResourceBarrier(m_ptrCmdLst.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE);

    m_ptrCmdLst->CopyResource(m_ptrSwapChainBackBuffers[m_iSwapChainBufferIndex].Get(), pRt->GetResource());

    d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier.Transition.pResource = m_ptrSwapChainBackBuffers[m_iSwapChainBufferIndex].Get();
    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_ptrCmdLst->ResourceBarrier(1, &d3dResourceBarrier);

    //m_ptrCmdLst->Close();
    //ID3D12CommandList* ppd3dCommandLists[] = { m_ptrCmdLst.Get() };
    //m_ptrCmdQueue->ExecuteCommandLists(1, ppd3dCommandLists);
    //
    //m_ptrDxgiSwapChain->Present(0, 0);
    //
    //WaitForGpuComplete();
    //MoveToNextFrame();
}

HRESULT Core::CreateDevice()
{
    //Enable DebugLayer
    UINT nDXGIFactoryFlags = 0;
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugController;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    if (debugController)
    {
        debugController->EnableDebugLayer();
    }
    nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

    //Create DxgiFactory for adaptor
    CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&m_ptrDxgiFactory));

    ComPtr<IDXGIAdapter1> Adapter;

    //Search Adapter good Device
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_ptrDxgiFactory->EnumAdapters1(i, &Adapter); ++i)
    {
        DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
        Adapter->GetDesc1(&dxgiAdapterDesc);
        if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_ptrDevice)))) break;
    }
    if (!m_ptrDevice)
    {
        m_ptrDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&Adapter));
        D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_ptrDevice));
    }
    if (!m_ptrDevice)
    {
        MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
        ::PostQuitMessage(0);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT Core::CreateFence()
{
    //Create Fence
    if (FAILED(m_ptrDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_ptrFence))))
        return E_FAIL;

    //Get IncrementSize
    gnRtvDescriptorIncrementSize = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    gnDsvDescriptorIncrementSize = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    gnCbvSrvUavDescriptorIncrementSize = m_ptrDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    return S_OK;
}

HRESULT Core::MsaaTest()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualLevels;

    msQualLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    msQualLevels.SampleCount = 4;
    msQualLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualLevels.NumQualityLevels = 0;

    //Check Feature Support Is Able MSAA?
    if(FAILED(m_ptrDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualLevels, sizeof(msQualLevels))))
        return E_FAIL;

    m_iMsaa4xQualityLevels = msQualLevels.NumQualityLevels;
    m_bMsaa4xEnable = (m_iMsaa4xQualityLevels > 1) ? true : false;

    return S_OK;
}

HRESULT Core::CreateCmdLstAndQueueAndAllocator()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    if (FAILED(m_ptrDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_ptrCmdQueue))))
        return E_FAIL;

    if (FAILED(m_ptrDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ptrCmdAllocator))))
        return E_FAIL;

    if (FAILED(m_ptrDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ptrCmdAllocatorForLoading))))
        return E_FAIL;
    
    if (FAILED(m_ptrDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_ptrCmdAllocator.Get(), NULL, IID_PPV_ARGS(&m_ptrCmdLst))))
        return E_FAIL;

    if (FAILED(m_ptrDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_ptrCmdAllocatorForLoading.Get(), NULL, IID_PPV_ARGS(&m_ptrCmdLstForLoading))))
        return E_FAIL;

    m_ptrCmdLst->Close();
    //m_ptrCmdLstForLoading->Close();

    return S_OK;
}

HRESULT Core::CreateSwapchain()
{
    //기존의 스왑체인을 해제한다.
    m_ptrDxgiSwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = WINCX;
    sd.BufferDesc.Height = WINCY;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    sd.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
    sd.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_iMsaa4xQualityLevels - 1) : 0;

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.OutputWindow = g_hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if(FAILED(m_ptrDxgiFactory->CreateSwapChain(m_ptrCmdQueue.Get(), &sd, (IDXGISwapChain**)(m_ptrDxgiSwapChain.GetAddressOf()))))
        return E_FAIL;

    m_iSwapChainBufferIndex = m_ptrDxgiSwapChain->GetCurrentBackBufferIndex();


    return S_OK;
}

HRESULT Core::CreateDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = 9;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvDesc.NodeMask = 0;

    if (FAILED(m_ptrDevice->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_ptrRTVHeap))))
        return E_FAIL;


    D3D12_DESCRIPTOR_HEAP_DESC dsvDesc;
    dsvDesc.NumDescriptors = 3;
    dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvDesc.NodeMask = 0;

    if (FAILED(m_ptrDevice->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&m_ptrDSVHeap))))
        return E_FAIL;

    return S_OK;
}

HRESULT Core::CreateRtvDsvBufferAndView()
{
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHwnd = m_ptrRTVHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < 2; ++i)
    {
        m_ptrDxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ptrSwapChainBackBuffers[i]));
        m_ptrDevice->CreateRenderTargetView(m_ptrSwapChainBackBuffers[i].Get(), &rtvDesc, rtvCpuDescriptorHwnd);
        m_SwapChainBackBufferRTVCPUHandles[i] = rtvCpuDescriptorHwnd;
        rtvCpuDescriptorHwnd.ptr += gnRtvDescriptorIncrementSize;
    }

    D3D12_RESOURCE_DESC rscDesc;
    rscDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    rscDesc.Alignment = 0;
    rscDesc.Width = WINCX;
    rscDesc.Height = WINCY;
    rscDesc.DepthOrArraySize = 1;
    rscDesc.MipLevels = 1;
    rscDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    rscDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
    rscDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_iMsaa4xQualityLevels - 1) : 0;
    rscDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    rscDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES heapPro;
    ZeroMemory(&heapPro, sizeof(D3D12_HEAP_PROPERTIES));
    heapPro.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapPro.CreationNodeMask = 1;
    heapPro.VisibleNodeMask = 1;

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_ptrDSVHeap->GetCPUDescriptorHandleForHeapStart();

    if (FAILED(m_ptrDevice->CreateCommittedResource(
        &heapPro, D3D12_HEAP_FLAG_NONE, &rscDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue, IID_PPV_ARGS(&m_ptrDsv)
    )))
        return E_FAIL;

    if (FAILED(m_ptrDevice->CreateCommittedResource(
        &heapPro, D3D12_HEAP_FLAG_NONE, &rscDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue, IID_PPV_ARGS(&m_ptrDsvForShade)
    )))
        return E_FAIL;

    if (FAILED(m_ptrDevice->CreateCommittedResource(
        &heapPro, D3D12_HEAP_FLAG_NONE, &rscDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue, IID_PPV_ARGS(&m_ptrDsvForShadow)
    )))
        return E_FAIL;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvViewDesc;
    ZeroMemory(&dsvViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    dsvViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvViewDesc.Flags = D3D12_DSV_FLAG_NONE;

    m_DsvCPUHandles = m_ptrDSVHeap->GetCPUDescriptorHandleForHeapStart();
    m_ptrDevice->CreateDepthStencilView(m_ptrDsv.Get(), &dsvViewDesc, d3dDsvCPUDescriptorHandle);

    d3dDsvCPUDescriptorHandle.ptr += gnDsvDescriptorIncrementSize;
    m_DsvForShadeCPUHandles = d3dDsvCPUDescriptorHandle;
    m_ptrDevice->CreateDepthStencilView(m_ptrDsvForShade.Get(), &dsvViewDesc, d3dDsvCPUDescriptorHandle);

    d3dDsvCPUDescriptorHandle.ptr += gnDsvDescriptorIncrementSize;
    m_DsvForShadowCPUHandles = d3dDsvCPUDescriptorHandle;
    m_ptrDevice->CreateDepthStencilView(m_ptrDsvForShadow.Get(), &dsvViewDesc, d3dDsvCPUDescriptorHandle);

    return S_OK;
}

HRESULT Core::SetViewportAndScissorrect()
{
    m_Viewport.TopLeftX = 0.f;
    m_Viewport.TopLeftY = 0.f;
    m_Viewport.Width = WINCX;
    m_Viewport.Height = WINCY;
    m_Viewport.MaxDepth = 1.f;
    m_Viewport.MinDepth = 0.f;

    m_ScissorRect.left = 0;
    m_ScissorRect.top = 0;
    m_ScissorRect.right = WINCX;
    m_ScissorRect.bottom = WINCY;

    return S_OK;
}

void Core::Present()
{
    m_ptrDxgiSwapChain->Present(0, 0);
}

void Core::CmdLstReset()
{
    m_ptrCmdLst->Reset(m_ptrCmdAllocator.Get(), nullptr);
}

void Core::CmdLstExecute()
{
    m_ptrCmdLst->Close();
    if(m_bLoadingThread)
    {
        ID3D12CommandList* cmdsLists[] = { m_ptrCmdLst.Get(), m_ptrCmdLstForLoading.Get() };
        m_ptrCmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
        m_bLoadingThread = false;
    }
    else
    {
        ID3D12CommandList* cmdsLists[] = { m_ptrCmdLst.Get() };
        m_ptrCmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    }

    
}

void Core::MoveToNextFrame()
{
    m_iSwapChainBufferIndex = m_ptrDxgiSwapChain->GetCurrentBackBufferIndex();

    UINT64 nFenceValue = ++m_nFenceValues[m_iSwapChainBufferIndex];
    HRESULT hResult = m_ptrCmdQueue->Signal(m_ptrFence.Get(), nFenceValue);
    
    if (m_ptrFence->GetCompletedValue() < nFenceValue)
    {
        hResult = m_ptrFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
        ::WaitForSingleObject(m_hFenceEvent, INFINITE);
    }
}

void Core::WaitForGpuComplete()
{
    const UINT64 nFenceValue = ++m_nFenceValues[m_iSwapChainBufferIndex];
    HRESULT hResult = m_ptrCmdQueue->Signal(m_ptrFence.Get(), nFenceValue);

    if (m_ptrFence->GetCompletedValue() < nFenceValue)
    {
        hResult = m_ptrFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
        ::WaitForSingleObject(m_hFenceEvent, INFINITE);
    }
}

void Core::CmdLstForLoadingReset()
{
    m_ptrCmdAllocatorForLoading->Reset();
    m_ptrCmdLstForLoading->Reset(m_ptrCmdAllocator.Get(), nullptr);
}

void Core::CmdLstForLoadingClose()
{
    m_bLoadingThread = true;
    m_ptrCmdLstForLoading->Close();
}

void Core::SetRenderTarget(const int& numRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE RtStartHandle, D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle)
{
    m_ptrCmdLst->OMSetRenderTargets(numRenderTarget, &RtStartHandle, TRUE, &DsvHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE Core::CreateRenderTargetView(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC rtvDesc)
{
    D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHwnd = m_ptrRTVHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE returnhandle = m_ptrRTVHeap->GetCPUDescriptorHandleForHeapStart();
    returnhandle.ptr += m_iRtvCnt * gnRtvDescriptorIncrementSize;
    rtvCpuDescriptorHwnd.ptr += m_iRtvCnt++ * gnRtvDescriptorIncrementSize;

    m_ptrDevice->CreateRenderTargetView(resource, &rtvDesc, rtvCpuDescriptorHwnd);
    return returnhandle;
}
