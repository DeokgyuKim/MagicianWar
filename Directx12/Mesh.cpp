#include "Mesh.h"
#include "MeshMgr.h"
#include "Renderer.h"

Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst,
	ID3D12DescriptorHeap* heap, 
	string _meshName)
	: Geometry(device, cmdLst, heap)
{
	MeshLoader = MeshMgr::GetInstnace();
	meshName = _meshName;
	m_CmdLst = cmdLst;
	TexTransform = MathHelper::Identity4x4();
	
	
	Initialize(device, heap);

}

Mesh::~Mesh()
{
	delete m_BoxGeo;
	m_BoxGeo = nullptr;
}


HRESULT Mesh::BuildGeometry(ID3D12Device* device)
{
	m_BoxGeo = MeshLoader->GetMesh(meshName);
	return S_OK;
}


