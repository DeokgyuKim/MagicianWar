#include "Terrain.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Plane.h"

Terrain::Terrain(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
    m_pRenderer = pRenderer;
	Initialize();
	
}

Terrain::~Terrain()
{
}

void Terrain::Initialize()
{
	for (int i = 0; i < TerrainZ; ++i)
		for (int j = 0; j < TerrainX; ++j)
			m_pBuffer[i][j] = new Plane(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), j, i);
	BuildConstantBuffer();

	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());
	m_strTextureName = "Stone01";
}

void Terrain::Release()
{
	for (int i = 0; i < TerrainZ; ++i)
		for (int j = 0; j < TerrainX; ++j)
		{
			if (m_pBuffer[i][j] != NULL)
				delete m_pBuffer[i][j];
			m_pBuffer[i][j] = NULL;
		}
}

HRESULT Terrain::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);

	//UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();
	//// Offset to the ith object constant buffer in the buffer.
	//int boxCBufIndex = 0;
	//cbAddress += boxCBufIndex * objCBByteSize;

	//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	//cbvDesc.BufferLocation = cbAddress;
	//cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));
	//
	//m_pRenderer->CreateConstantBufferView(cbvDesc);
	return S_OK;
}

int Terrain::Update(const float& fTimeDelta)
{
	return 0;
}

void Terrain::LateUpdate(const float& fTimeDelta)
{
	XMMATRIX world = XMLoadFloat4x4(&m_xmmWorld);// *view* proj;
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(world));


	m_ObjectCB->CopyData(0, ObjCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_NOBLEND, this);
}

void Terrain::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < TerrainZ; ++i)
		for (int j = 0; j < TerrainX; ++j)
			m_pBuffer[i][j]->Render(fTimeDelta);
}
