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
#include "Skybox.h"
#include "InstanceInfo.h"
#include "MaterialMgr.h"
#include "InstanceMgr.h"

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

	m_pLight = new CLight(m_pDevice, m_pCmdLst, m_ptrDescriptorHeap.Get(), this,
		XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(-50.f, 50.f, -50.f, 1.f)
		, XMFLOAT4(1.f, -1.f, 1.f, 0.f));
	InitializeCriticalSection(&m_Crt);
	//BuildTextures();
}

void Renderer::Render(const float& fTimeDelta)
{
	//EnterCriticalSection(&m_Crt);
	m_pCore->Render_Begin();

	m_pRTMgr->ClearMultiRenderTarget(m_pCmdLst, "Deffered");
	m_pRTMgr->SetMultiRenderTarget(m_pCmdLst, "Deffered", m_pCore->GetDSVCpuHandle());

    ////////////////////////////Render////////////////////////////
	m_pCmdLst->SetGraphicsRootSignature(m_ptrRootSignature.Get());


	ID3D12DescriptorHeap* descriptorHeaps[] = { m_ptrDescriptorHeap.Get() };
	m_pCmdLst->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	MaterialMgr::GetInstnace()->SetGraphicsShaderResourceView(); // 1¹øÀÇ MatBuffer SRV ¿¬°á

	m_pCamera->Render(0.f);

	//m_mapShaders[RENDER_TYPE::RENDER_COLOR]->PreRender(m_pCmdLst);
	//for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_COLOR])
	//{
	//	pObject->Render(fTimeDelta);

	//}
	////DrawObject(,m_lstObjects[RENDER_TYPE::RENDER_COLOR])

	////Set Pipeline
	m_mapShaders[RENDER_TYPE::RENDER_NOBLEND]->PreRender(m_pCmdLst);
	 
	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_NOBLEND])
	{
		if (m_InstanceCheck.find(pObject->GetMeshName()) != m_InstanceCheck.end()) {
			// ÀÌ¹Ì ÇÑ¹ø ·»´õ¸µ ÇÑ mesh¸é ·»´õ¸µ¾ÈÇÏ¸éµÊ
		}
		else
		{
			if (pObject->GetTextureName() != "")
				m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
			m_pCmdLst->SetGraphicsRootShaderResourceView(0, InstanceMgr::GetInstnace()->m_InstanceCBs[pObject->GetMeshName()]->Resource()->GetGPUVirtualAddress());	// obj
			m_InstanceCheck[pObject->GetMeshName()] = InstanceMgr::GetInstnace()->m_InstanceObjects[pObject->GetMeshName()]->GetInstanceCount();
			pObject->Render(fTimeDelta, m_InstanceCheck[pObject->GetMeshName()]);
		}
	}

	//m_mapShaders[RENDER_TYPE::RENDER_BULLET]->PreRender(m_pCmdLst);
	//for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_BULLET])
	//{
	//	m_pTextureMgr->GetTexture("Noise3")->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
	//	if (pObject->GetTextureName() != "")
	//		m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
	//	pObject->Render(fTimeDelta);
	//}

	m_mapShaders[RENDER_TYPE::RENDER_STATIC]->PreRender(m_pCmdLst);
	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_STATIC])
	{
		if (m_InstanceCheck.find(pObject->GetMeshName()) != m_InstanceCheck.end()) {
			// ÀÌ¹Ì ÇÑ¹ø ·»´õ¸µ ÇÑ mesh¸é ·»´õ¸µ¾ÈÇÏ¸éµÊ
		}
		else
		{
			if (pObject->GetTextureName() != "")
				m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
			m_pCmdLst->SetGraphicsRootShaderResourceView(0, InstanceMgr::GetInstnace()->m_InstanceCBs[pObject->GetMeshName()]->Resource()->GetGPUVirtualAddress());	// obj
			m_InstanceCheck[pObject->GetMeshName()] = InstanceMgr::GetInstnace()->m_InstanceObjects[pObject->GetMeshName()]->GetInstanceCount();
			pObject->Render(fTimeDelta, m_InstanceCheck[pObject->GetMeshName()]);
		}
	}

	m_mapShaders[RENDER_TYPE::RENDER_DYNAMIC]->PreRender(m_pCmdLst);
	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_DYNAMIC])
	{
		if (m_InstanceCheck.find(pObject->GetMeshName()) != m_InstanceCheck.end()) {
			// ÀÌ¹Ì ÇÑ¹ø ·»´õ¸µ ÇÑ mesh¸é ·»´õ¸µ¾ÈÇÏ¸éµÊ
		}
		else
		{
			if (pObject->GetTextureName() != "")
				m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
			m_pCmdLst->SetGraphicsRootShaderResourceView(0, InstanceMgr::GetInstnace()->m_InstanceCBs[pObject->GetMeshName()]->Resource()->GetGPUVirtualAddress());	// obj
			m_InstanceCheck[pObject->GetMeshName()] = InstanceMgr::GetInstnace()->m_InstanceObjects[pObject->GetMeshName()]->GetInstanceCount();
			pObject->Render(fTimeDelta, m_InstanceCheck[pObject->GetMeshName()]);
		}
	}

	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_SKILL])
	{
		m_pTextureMgr->GetTexture("Noise3")->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
		pObject->Render(fTimeDelta);
	}

    //////////////////////////////////////////////////////////////

	m_pRTMgr->ClearMultiRenderTarget(m_pCmdLst, "Shade");
	m_pRTMgr->SetMultiRenderTarget(m_pCmdLst, "Shade", m_pCore->GetDSVForShadeCpuHandle());
	m_mapShaders[RENDER_TYPE::RENDER_SHADE]->PreRender(m_pCmdLst);

	m_pRTMgr->GetRenderTarget("Diffuse")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 5);
	m_pRTMgr->GetRenderTarget("Ambient")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 6);
	m_pRTMgr->GetRenderTarget("Specular")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 7);
	m_pRTMgr->GetRenderTarget("Normal")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 8);
	m_pRTMgr->GetRenderTarget("Depth")->SetShaderVariable(m_pCmdLst, m_ptrDescriptorHeap.Get(), 9);

	m_pLight->RenderLight();

	m_pRTMgr->SetMultiRenderTarget(m_pCmdLst, "Shade", m_pCore->GetDSVCpuHandle());
	m_mapShaders[RENDER_TYPE::RENDER_SKYBOX]->PreRender(m_pCmdLst);
	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_SKYBOX])
	{
		if (m_InstanceCheck.find(pObject->GetMeshName()) != m_InstanceCheck.end()) {
			// ÀÌ¹Ì ÇÑ¹ø ·»´õ¸µ ÇÑ mesh¸é ·»´õ¸µ¾ÈÇÏ¸éµÊ
		}
		else
		{
			if (pObject->GetTextureName() != "")
				m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
			m_pCmdLst->SetGraphicsRootShaderResourceView(0, InstanceMgr::GetInstnace()->m_InstanceCBs[pObject->GetMeshName()]->Resource()->GetGPUVirtualAddress());	// obj
			m_InstanceCheck[pObject->GetMeshName()] = InstanceMgr::GetInstnace()->m_InstanceObjects[pObject->GetMeshName()]->GetInstanceCount();
			pObject->Render(fTimeDelta, m_InstanceCheck[pObject->GetMeshName()]);
		}
	}


	m_mapShaders[RENDER_TYPE::RENDER_UI]->PreRender(m_pCmdLst);
	for (auto pObject : m_lstObjects[RENDER_TYPE::RENDER_UI])
	{
		if (pObject->GetTextureName() != "")
			m_pTextureMgr->GetTexture(pObject->GetTextureName())->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
		pObject->Render(fTimeDelta);
	}

	DebugKeyInput();
	m_pCore->Render_EndTest(m_pRTMgr->GetRenderTarget(DebugInput));
	//EnterCriticalSection(&m_Crt);

	m_pCore->CmdLstExecute();

	m_pCore->Present();

	m_pCore->WaitForGpuComplete();
	m_pCore->MoveToNextFrame();

	//m_pCore->Render_End();
	for(int i = 0; i < RENDER_TYPE::RENDER_END; ++i)
		m_lstObjects[i].clear();

	m_InstanceCheck.clear();

}

void Renderer::DrawObject(InstanceInfo* _inst, list<Object*> _Objects)
{
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
	CD3DX12_DESCRIPTOR_RANGE srvTable[13];
	srvTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // DDStexture
	srvTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // Diff Texture
	srvTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // Ambi Texture
	srvTable[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3); // Specular Texture
	srvTable[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4); // Normal Texture
	srvTable[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5); // Depth Texture
	srvTable[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6); // CubeMap Texture
	srvTable[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7); // Noise Texture
	srvTable[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);	// Skill Effect Texture1
	srvTable[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9);	// Skill Effect Texture2
	srvTable[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10);	// Skill Effect Texture3
	srvTable[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11);	// Skill Effect Texture4
	srvTable[12].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 12);	// Skill Effect Texture5


	const size_t rootSize = 19;

	CD3DX12_ROOT_PARAMETER slotRootParameter[rootSize];


	slotRootParameter[0].InitAsShaderResourceView(0, 1);	// Instance Data
	slotRootParameter[1].InitAsConstantBufferView(1);	//view, proj, invview, invproj, campos
	slotRootParameter[2].InitAsShaderResourceView(1, 1); // Material diffuse, ambient, specular, position
	slotRootParameter[3].InitAsConstantBufferView(3);	//light diffuse, ambient, specular, position, direction
	slotRootParameter[4].InitAsDescriptorTable(1, &srvTable[0], D3D12_SHADER_VISIBILITY_PIXEL);	//Object texture
	slotRootParameter[5].InitAsDescriptorTable(1, &srvTable[1], D3D12_SHADER_VISIBILITY_PIXEL);	//diffuse render target
	slotRootParameter[6].InitAsDescriptorTable(1, &srvTable[2], D3D12_SHADER_VISIBILITY_PIXEL);	//ambient render target
	slotRootParameter[7].InitAsDescriptorTable(1, &srvTable[3], D3D12_SHADER_VISIBILITY_PIXEL);	//specular render target
	slotRootParameter[8].InitAsDescriptorTable(1, &srvTable[4], D3D12_SHADER_VISIBILITY_PIXEL);	//normal render target
	slotRootParameter[9].InitAsDescriptorTable(1, &srvTable[5], D3D12_SHADER_VISIBILITY_ALL);	//depth render target
	slotRootParameter[10].InitAsDescriptorTable(1, &srvTable[6], D3D12_SHADER_VISIBILITY_ALL);	//Skybox texture
	slotRootParameter[11].InitAsConstantBufferView(4); // Skinned
	slotRootParameter[12].InitAsDescriptorTable(1, &srvTable[7], D3D12_SHADER_VISIBILITY_ALL);		//Noise texture
	slotRootParameter[13].InitAsDescriptorTable(1, &srvTable[8], D3D12_SHADER_VISIBILITY_PIXEL);	// Skill Effect Texture1
	slotRootParameter[14].InitAsDescriptorTable(1, &srvTable[9], D3D12_SHADER_VISIBILITY_PIXEL);	// Skill Effect Texture2
	slotRootParameter[15].InitAsDescriptorTable(1, &srvTable[10], D3D12_SHADER_VISIBILITY_PIXEL);	// Skill Effect Texture3
	slotRootParameter[16].InitAsDescriptorTable(1, &srvTable[11], D3D12_SHADER_VISIBILITY_PIXEL);	// Skill Effect Texture4
	slotRootParameter[17].InitAsDescriptorTable(1, &srvTable[12], D3D12_SHADER_VISIBILITY_PIXEL);	// Skill Effect Texture5
	slotRootParameter[18].InitAsConstantBufferView(5);	//light diffuse, ambient, specular, position, direction
	

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(rootSize, slotRootParameter,
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
	srvHeapDesc.NumDescriptors = 33;// ÅØ½ºÃ³ °³¼ö + blur skybox µîµî
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
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 1, true, false);
	m_mapShaders[RENDER_TYPE::RENDER_SHADE] = pShader;

	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_UI", L"color.hlsl", "PS_UI", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 1, true, false);
	m_mapShaders[RENDER_TYPE::RENDER_UI] = pShader;

	// Static Model
	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_Static", L"color.hlsl", "PS_Static", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 5);
	m_mapShaders[RENDER_TYPE::RENDER_STATIC] = pShader;

	// Skinned Model
	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 68, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_Movable", L"color.hlsl", "PS_Movable", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 5);
	m_mapShaders[RENDER_TYPE::RENDER_DYNAMIC] = pShader;

	// SkyBox
	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_Skybox", L"color.hlsl", "PS_Skybox", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 1, false);
	m_mapShaders[RENDER_TYPE::RENDER_SKYBOX] = pShader;


	// Static Model
	pShader = new Shader;
	layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	pShader->BuildShadersAndInputLayout(L"color.hlsl", "VS_FireBall", L"color.hlsl", "PS_FireBall", layout);
	pShader->BuildPipelineState(m_pDevice, m_ptrRootSignature.Get(), 5);
	m_mapShaders[RENDER_TYPE::RENDER_BULLET] = pShader;
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

void Renderer::DebugKeyInput()
{
	if (GetAsyncKeyState(VK_F4) & 0x8000)
		DebugInput = "Shade";
	if (GetAsyncKeyState(VK_F5) & 0x8000)
		DebugInput = "Diffuse";
	if (GetAsyncKeyState(VK_F6) & 0x8000)
		DebugInput = "Ambient";
	if (GetAsyncKeyState(VK_F7) & 0x8000)
		DebugInput = "Specular";
	if (GetAsyncKeyState(VK_F8) & 0x8000)
		DebugInput = "Normal";
	if (GetAsyncKeyState(VK_F9) & 0x8000)
		DebugInput = "Depth";
}
