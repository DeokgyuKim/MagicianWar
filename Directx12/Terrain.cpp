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

	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());
	m_strTextureName = "Stone01";

	m_mapComponent["Material"] = new MaterialCom(XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f), XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f), 
		XMFLOAT4(0.9f, 0.9f, 0.9f, 0.9f));
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
	m_MaterialCB = make_unique<UploadBuffer<MaterialCB>>(m_pDevice, 1, true);
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
	XMStoreFloat4x4(&ObjCB.WorldNoScaling, XMMatrixTranspose(world));
	m_ObjectCB->CopyData(0, ObjCB);


	MaterialCB	MatCB;
	XMStoreFloat4(&MatCB.Diffuse, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetDiffuse());
	XMStoreFloat4(&MatCB.Ambient, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetAmbient());
	XMStoreFloat4(&MatCB.Specular, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetSpecular());
	m_MaterialCB->CopyData(0, MatCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_NOBLEND, this);
}

void Terrain::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(2, m_MaterialCB->Resource()->GetGPUVirtualAddress());

	for (int i = 0; i < TerrainZ; ++i)
		for (int j = 0; j < TerrainX; ++j)
			m_pBuffer[i][j]->Render(fTimeDelta);
}
