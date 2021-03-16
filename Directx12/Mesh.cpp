#include "Mesh.h"
#include "MeshMgr.h"
#include "Renderer.h"

Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst,
	ID3D12DescriptorHeap* heap, 
	string strFilePath, MESH_TYPE etype)
	: Geometry(device, cmdLst, heap)
{
	MeshLoader = MeshMgr::GetInstnace();
	meshName = strFilePath;
	m_CmdLst = cmdLst;
	switch (etype)
	{
	case MESH_TYPE::CHARACTER:
		m_strFilePath = "..//Resources//Models//Characters//";
		break;
	case MESH_TYPE::COUNT:
		break;
	default:
		break;
	}
	m_strFilePath += strFilePath + "//"+ strFilePath;

	TexTransform = MathHelper::Identity4x4();
	
	BuildMaterials();
	Initialize(device, heap);

}

Mesh::~Mesh()
{
}


HRESULT Mesh::BuildGeometry(ID3D12Device* device)
{
	vector<SkinnedVertex>	skinnedVertices;
	vector<UINT>			indices;
	vector<Material>		materials;
	unique_ptr<SkinnedData> skinnedInfo = make_unique<SkinnedData>();
	
	bool check = true;
	check = MeshLoader->LoadMeshFile(skinnedVertices,indices,&materials, m_strFilePath);
	if (check) { cout << "메쉬 로드 실패" << endl; }
	check = MeshLoader->LoadSkeletonFile(*skinnedInfo, m_strFilePath);
	if (check) { cout << "스켈레톤 로드 실패" << endl; }

	unique_ptr<SkinnedModelInstance> skinnedModelInst = make_unique<SkinnedModelInstance>();
	skinnedModelInst->SkinnedInfo = move(skinnedInfo);
	skinnedModelInst->Transforms.resize(skinnedModelInst->SkinnedInfo->BoneCount());
	m_SkinnedModelInst = move(skinnedModelInst);

	const UINT vbByteSize = (UINT)skinnedVertices.size() * sizeof(SkinnedVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint32_t);

	m_BoxGeo = make_unique<MeshGeometry>();
	m_BoxGeo->Name = "BufferGeo"; // 메시 이름

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), skinnedVertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		m_CmdLst.Get(), skinnedVertices.data(), vbByteSize, m_BoxGeo->VertexBufferUploader);

	m_BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		m_CmdLst.Get(), indices.data(), ibByteSize, m_BoxGeo->IndexBufferUploader);

	m_BoxGeo->VertexByteStride = sizeof(SkinnedVertex);
	m_BoxGeo->VertexBufferByteSize = vbByteSize;
	m_BoxGeo->IndexFormat = DXGI_FORMAT_R32_UINT;
	m_BoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	
	m_BoxGeo->DrawArgs["BufferGeo"] = submesh;

	return S_OK;
}

void Mesh::BuildMaterials()
{
	auto material = make_unique<Material>();
	material->MatCBIndex = 1;
	material->DiffuseSrvHeapIndex = 1;
	material->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	material->Roughness = 0.3f;

	m_Material = move(material);
}


