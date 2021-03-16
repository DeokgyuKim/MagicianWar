#include "Renderer.h"
#include "Core.h"
#include "Object.h"
#include "Shader.h"
#include "DDSTexture.h"
#include "Camera.h"
#include "TextureMgr.h"
#include "RenderTarget.h"
#include "RenderTargetMgr.h"
#include "CLight.h"

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

	m_pLight = new CLight(m_pDevice, m_pCmdLst, m_ptrDescriptorHeap.Get(), this,
		XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.f, 0.f, 0.f, 1.f), XMFLOAT4(1.f, -1.f, 1.f, 1.f));
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

	m_pRTMgr->ClearMultiRenderTarget(m_pCmdLst, "Shade");
	m_pRTMgr->SetMultiRenderTarget(m_pCmdLst, "Shade");
	m_mapShaders[RENDER_TYPE::RENDER_SHADE]->PreRender(m_pCmdLst);

	m_pRTMgr->GetRenderTarget("Diffuse")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 12);
	m_pRTMgr->GetRenderTarget("Ambient")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 13);
	m_pRTMgr->GetRenderTarget("Specular")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 14);
	m_pRTMgr->GetRenderTarget("Normal")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 15);
	m_pRTMgr->GetRenderTarget("Depth")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 16);

	m_pLight->RenderLight();


	m_pCore->Render_EndTest(m_pRTMgr->GetRenderTarget("Shade"));

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
	CD3DX12_DESCRIPTOR_RANGE srvTable[6];
	srvTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	srvTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	srvTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	srvTable[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
	srvTable[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
	srvTable[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);


	CD3DX12_ROOT_PARAMETER slotRootParameter[20];


	slotRootParameter[0].InitAsConstantBufferView(0);	//world
	slotRootParameter[1].InitAsConstantBufferView(1);	//view
	slotRootParameter[2].InitAsConstantBufferView(2);	//proj
	slotRootParameter[3].InitAsConstantBufferView(3);	//diffuse
	slotRootParameter[4].InitAsConstantBufferView(4);	//ambient
	slotRootParameter[5].InitAsConstantBufferView(5);	//specular
	slotRootParameter[6].InitAsConstantBufferView(6);	//light diffuse
	slotRootParameter[7].InitAsConstantBufferView(7);	//light ambient
	slotRootParameter[8].InitAsConstantBufferView(8);	//light specular
	slotRootParameter[9].InitAsConstantBufferView(9);	//light position
	slotRootParameter[10].InitAsConstantBufferView(10);	//light direction
	slotRootParameter[11].InitAsDescriptorTable(1, &srvTable[0], D3D12_SHADER_VISIBILITY_PIXEL);	//Object texture
	slotRootParameter[12].InitAsDescriptorTable(1, &srvTable[1], D3D12_SHADER_VISIBILITY_PIXEL);	//diffuse render target
	slotRootParameter[13].InitAsDescriptorTable(1, &srvTable[2], D3D12_SHADER_VISIBILITY_PIXEL);	//ambient render target
	slotRootParameter[14].InitAsDescriptorTable(1, &srvTable[3], D3D12_SHADER_VISIBILITY_PIXEL);	//specular render target
	slotRootParameter[15].InitAsDescriptorTable(1, &srvTable[4], D3D12_SHADER_VISIBILITY_PIXEL);	//normal render target
	slotRootParameter[16].InitAsDescriptorTable(1, &srvTable[5], D3D12_SHADER_VISIBILITY_ALL);		//depth render target
	//slotRootParameter[17].InitAsConstantBufferView(11);	//camera direction
	slotRootParameter[17].InitAsConstantBufferView(11);	//camera inv view
	slotRootParameter[18].InitAsConstantBufferView(12);	//camera inv proj
	slotRootParameter[19].InitAsConstantBufferView(13);	//camera position

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(20, slotRootParameter,
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
	srvHeapDesc.NumDescriptors = 33;///////////////////////
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
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 5);
	m_mapShaders[RENDER_TYPE::RENDER_COLOR] = pShader;

	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_Main", L"color.hlsl", "PS_Main", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 5);
	m_mapShaders[RENDER_TYPE::RENDER_NOBLEND] = pShader;

	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_Shade", L"color.hlsl", "PS_Shade", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 1);
	m_mapShaders[RENDER_TYPE::RENDER_SHADE] = pShader;
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