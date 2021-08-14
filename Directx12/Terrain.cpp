#include "Terrain.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Plane.h"
#include "Material.h"
#include "Transform.h"

Terrain::Terrain(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
	string _meshName)
	:Object(_meshName)
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

	XMFLOAT3 scale = XMFLOAT3(50.f, 1.f, 100.f);
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixScalingFromVector(XMLoadFloat3(&scale)) * XMMatrixTranslation(25.f, 0.f, 50.f));
	m_strTextureName = "Terrain";
	m_mapComponent["Transform"] = new Transform(scale, XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(25.f, 0.f, 50.f));

	m_mapComponent["Material"] = new MaterialCom("Stone01",XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f), XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f), 
		XMFLOAT4(0.f, 0.f, 0.f, 1.f));

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
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

int Terrain::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);
	return 0;
}

void Terrain::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);
	m_pRenderer->PushObject(RENDER_TYPE::RENDER_NOBLEND, this);
}

void Terrain::Render(const float& fTimeDelta, int _instanceCount)
{
	for (int i = 0; i < TerrainZ; ++i)
		for (int j = 0; j < TerrainX; ++j)
			m_pBuffer[i][j]->Render(fTimeDelta, _instanceCount);
}
