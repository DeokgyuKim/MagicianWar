#include "CLight.h"
#include "LightGeo.h"
#include "Renderer.h"
#include "Core.h"

CLight::CLight(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, Renderer* pRenderer,
	XMFLOAT4 diff, XMFLOAT4 ambi, XMFLOAT4 spec, XMFLOAT4 pos, XMFLOAT4 dir)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pDescriptorHeap = heap;

	m_pRenderer = pRenderer;

	m_xmfDiffuse = diff;
	m_xmfAmbient = ambi;
	m_xmfSpecular = spec;
	m_xmfPosition = pos;
	m_xmfDirection = dir;

	Initialize();
}

CLight::~CLight()
{
}

void CLight::Initialize()
{
	Core::GetInstance()->CmdLstReset();
	m_pLightGeo = new LightGeo(m_pDevice, m_pCmdLst, m_pDescriptorHeap, WINCX, WINCY);
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	BuildConstantBuffer();
}

void CLight::BuildConstantBuffer()
{
	int boxCBufIndex = 0;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	///Diffuse
	m_DiffuseCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));


	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_DiffuseCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);

	///Ambient
	m_AmbientCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_AmbientCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);

	///Specular
	m_SpecularCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_SpecularCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);

	///Position
	m_PositionCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_PositionCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);

	///Specular
	m_DirectionCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_DirectionCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);


	MatCB		DiffCB;
	XMStoreFloat4(&DiffCB.Material, XMLoadFloat4(&m_xmfDiffuse));
	m_DiffuseCB->CopyData(0, DiffCB);

	MatCB		AmbiCB;
	XMStoreFloat4(&AmbiCB.Material, XMLoadFloat4(&m_xmfAmbient));
	m_AmbientCB->CopyData(0, AmbiCB);

	MatCB		SpecCB;
	XMStoreFloat4(&SpecCB.Material, XMLoadFloat4(&m_xmfSpecular));
	m_SpecularCB->CopyData(0, SpecCB);

	MatCB		PosCB;
	XMStoreFloat4(&PosCB.Material, XMLoadFloat4(&m_xmfPosition));
	m_PositionCB->CopyData(0, PosCB);

	MatCB		DirCB;
	XMStoreFloat4(&DirCB.Material, XMLoadFloat4(&m_xmfDirection));
	m_DirectionCB->CopyData(0, DirCB);
}

void CLight::SetLightMaterial()
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(6, m_DiffuseCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(7, m_AmbientCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(8, m_SpecularCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(9, m_PositionCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(10, m_DirectionCB->Resource()->GetGPUVirtualAddress());
}

void CLight::RenderLight()
{
	SetLightMaterial();
	m_pLightGeo->Render(0.f);
}
