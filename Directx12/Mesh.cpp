#include "Mesh.h"

#include "Renderer.h"

Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, string strFilePath)
	: Geometry(device, cmdLst, heap)
{
	
	m_CmdLst = cmdLst;
	Initialize(device, heap);
	m_strFilePath = strFilePath;
}

Mesh::~Mesh()
{
}


HRESULT Mesh::BuildGeometry(ID3D12Device* device)
{
	//MeshLoader::LoadMesh(m_strFilePath);
	//int a = MeshLoader::GetVertexCnt(m_strFilePath);
	//std::array<ColorVertex, a> vertices = MeshLoader::GetVertexes(m_strFilePath);
	//
	//
	//int b = MeshLoader::GetIndexCnt(m_strFilePath);
	//std::array<std::uint16_t, b> indices = MeshLoader::GetIndexes(m_strFilePath);
	//
	//const UINT vbByteSize = (UINT)vertices.size() * sizeof(ColorVertex);
	//const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);
	//
	//m_BoxGeo = make_unique<MeshGeometry>();
	//m_BoxGeo->Name = "BufferGeo";
	//
	//ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	//CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	//
	//ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	//CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
	//
	//m_BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
	//	m_CmdLst.Get(), vertices.data(), vbByteSize, m_BoxGeo->VertexBufferUploader);
	//
	//m_BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
	//	m_CmdLst.Get(), indices.data(), ibByteSize, m_BoxGeo->IndexBufferUploader);
	//
	//m_BoxGeo->VertexByteStride = sizeof(ColorVertex);
	//m_BoxGeo->VertexBufferByteSize = vbByteSize;
	//m_BoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	//m_BoxGeo->IndexBufferByteSize = ibByteSize;
	//
	//SubmeshGeometry submesh;
	//submesh.IndexCount = (UINT)indices.size();
	//submesh.StartIndexLocation = 0;
	//submesh.BaseVertexLocation = 0;
	//
	//m_BoxGeo->DrawArgs["BufferGeo"] = submesh;
	return S_OK;
}

