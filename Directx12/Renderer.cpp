#include "Renderer.h"
#include "Core.h"
#include "Object.h"
#include "Shader.h"
#include "DDSTexture.h"
#include "Camera.h"
#include "TextureMgr.h"
#include "RenderTarget.h"
#include "RenderTargetMgr.h"

Renderer* Renderer::m_pInstance = NULL;
Renderer* Renderer::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new Renderer;
	return m_pInstance;
}

void Renderer::DestroyInstance()
{
	if (m_pInstance != NULL)
		delete m_pInstance;
	m_pInstance = NULL;
}

void Renderer::InitRenderer(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst)
{
	m_pCore = pCore;
    m_pDevice = pDevice;
	m_pCmdLst = pCmdLst;

	BuildRootSignature();
	BuildDescrpitorHeap();
	m_pRTMgr = RenderTargetMgr::GetInstance();
	m_pRTMgr->BuildRenderTarget(m_pDevice, this);
	BuildShader();
	m_pTextureMgr = TextureMgr::GetInstance();
	m_pTextureMgr->BuildTextures(m_pDevice, m_pCmdLst, m_ptrDescriptorHeap.Get());
	//BuildTextures();
}

void Renderer::Render(const float& fTimeDelta)
{
	m_pCore->Render_Begin();

	m_pRTMgr->ClearMultiRenderTarget(m_pCmdLst, "Deffered");
	m_pRTMgr->SetMultiRenderTarget(m_pCmdLst, "Deffered");

    ////////////////////////////Render////////////////////////////
	m_pCmdLst->SetGraphicsRootSignature(m_ptrRootSignature.Get());


	ID3D12DescriptorHeap* descriptorHeaps[] = { m_ptrDescriptorHeap.Get() };
	m_pCmdLst->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	m_pCamera->Render(0.f);

	m_mapShaders[RENDER_TYPE::RENDER_COLOR]->PreRender(m_pCmdLst);

	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_COLOR])
	{
		pObject->Render(fTimeDelta);
	}

	//Set Pipeline
	m_mapShaders[RENDER_TYPE::RENDER_NOBLEND]->PreRender(m_pCmdLst);
	 
	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_NOBLEND])
	{
		if (pObject->GetTextureName() != "")
			m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
		pObject->Render(fTimeDelta);
	}
    //////////////////////////////////////////////////////////////

	m_pCore->Render_EndTest(m_pRTMgr->GetRenderTarget("Normal"));

	//m_pCore->Render_End();
	for(int i = 0; i < RENDER_TYPE::RENDER_END; ++i)
		m_lstObjects[i].clear();

}

void Renderer::PushObject(RENDER_TYPE eType, Object* pObject)
{
	m_lstObjects[eType].push_back(pObject);
}

void Renderer::CreateConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_ptrDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_iCountView++ * gnCbvSrvUavDescriptorIncrementSize;

	m_pDevice->CreateConstantBufferView(&cbvDesc, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::CreateShaderResourceView(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_ptrDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE returnhandle = m_ptrDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	returnhandle.ptr += m_iCountView * gnCbvSrvUavDescriptorIncrementSize;
	handle.ptr += m_iCountView++ * gnCbvSrvUavDescriptorIncrementSize;

	m_pDevice->CreateShaderResourceView(resource, &viewDesc, handle);
	return returnhandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::CreateUnorderedAccessView(ID3D12Resource* resource, D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_ptrDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE returnhandle = m_ptrDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	returnhandle.ptr += m_iCountView * gnCbvSrvUavDescriptorIncrementSize;
	handle.ptr += m_iCountView++ * gnCbvSrvUavDescriptorIncrementSize;

	m_pDevice->CreateUnorderedAccessView(resource, nullptr, &uavDesc, handle);
	return returnhandle;
}

void Renderer::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE srvTable;
	srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);


	CD3DX12_ROOT_PARAMETER slotRootParameter[4];


	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsConstantBufferView(2);
	slotRootParameter[3].InitAsDescriptorTable(1, &srvTable, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_pDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_ptrRootSignature)));

	return;
}

void Renderer::BuildDescrpitorHeap()
{
	//Create SRV Heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 10;///////////////////////
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_ptrDescriptorHeap));
}

void Renderer::BuildShader()
{
	Shader* pShader = NULL;

	pShader = new Shader;
	vector<D3D12_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS", L"color.hlsl", "PS", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get());
	m_mapShaders[RENDER_TYPE::RENDER_COLOR] = pShader;

	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_Main", L"color.hlsl", "PS_Main", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get());
	m_mapShaders[RENDER_TYPE::RENDER_NOBLEND] = pShader;
}
std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Renderer::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}