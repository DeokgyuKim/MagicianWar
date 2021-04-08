#include "Terrain.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Plane.h"
#include "Material.h"

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

	XMFLOAT3 scale = XMFLOAT3(50.f, 1.f, 100.f);
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixScalingFromVector(XMLoadFloat3(&scale)) * XMMatrixTranslation(24.5f, 0.f, 49.5f));
	m_strTextureName = "Stone01";

	m_mapComponent["Material"] = new MaterialCom("Stone01",XMFLOAT4(0.488f, 0.388f, 0.188f, 1.f), XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f), 
		XMFLOAT4(0.f, 0.f, 0.f, 1.f));
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
	return S_OK;
}

void Terrain::UpdateObjectCB()
{
	XMMATRIX world = XMLoadFloat4x4(&m_xmmWorld);// *view* proj;
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(world));
	ObjCB.MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
	m_ObjectCB->CopyData(0, ObjCB);
}

int Terrain::Update(const float& fTimeDelta)
{
	return 0;
}

void Terrain::LateUpdate(const float& fTimeDelta)
{
	UpdateObjectCB();

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_NOBLEND, this);
}

void Terrain::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < TerrainZ; ++i)
		for (int j = 0; j < TerrainX; ++j)
			m_pBuffer[i][j]->Render(fTimeDelta);
}
