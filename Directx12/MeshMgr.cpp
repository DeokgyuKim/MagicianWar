#include "MeshMgr.h"
#include "MaterialMgr.h"
#include <fstream>
MeshMgr* MeshMgr::m_pInstance = nullptr;

void MeshMgr::InitMeshMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst)
{
	m_pCore = pCore;
	m_pDevice = pDevice;
	m_pCmdLst = pCmdLst;
	MaterialLoader = MaterialMgr::GetInstnace();
}

void MeshMgr::BuildSkinnedModel(string meshName, MESH_TYPE etype)
{
	if (m_Meshs.count(meshName)) {
		cout << "중복 모델 로드" << endl;
		return;
	}

	// 메쉬가 없다면
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
	m_strFilePath += meshName + "//" + meshName;

	vector<SkinnedVertex>	skinnedVertices;
	vector<UINT>			indices;
	vector<Material>		materials;
	unique_ptr<SkinnedData> skinnedInfo = make_unique<SkinnedData>();

	bool check = true;
	check = LoadMeshFile(skinnedVertices, indices, &materials, m_strFilePath);
	if (!check) { cout << "메쉬 로드 실패" << endl; }
	check = LoadSkeletonFile(*skinnedInfo, m_strFilePath);
	if (!check) { cout << "스켈레톤 로드 실패" << endl; }

	unique_ptr<SkinnedModelInstance> skinnedModelInst = make_unique<SkinnedModelInstance>();
	skinnedModelInst->SkinnedInfo = move(skinnedInfo);
	skinnedModelInst->Transforms.resize(skinnedModelInst->SkinnedInfo->BoneCount());
	m_SkinnedModelInst[meshName] = move(skinnedModelInst);

	const UINT vbByteSize = (UINT)skinnedVertices.size() * sizeof(SkinnedVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT);

	auto meshGeo = make_unique<MeshGeometry>();
	meshGeo->Name = "BufferGeo"; // 메시 이름

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &meshGeo->VertexBufferCPU));
	CopyMemory(meshGeo->VertexBufferCPU->GetBufferPointer(), skinnedVertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &meshGeo->IndexBufferCPU));
	CopyMemory(meshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	meshGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_pDevice,
		m_pCmdLst, skinnedVertices.data(), vbByteSize, meshGeo->VertexBufferUploader);

	meshGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_pDevice,
		m_pCmdLst, indices.data(), ibByteSize, meshGeo->IndexBufferUploader);

	meshGeo->VertexByteStride = sizeof(SkinnedVertex);
	meshGeo->VertexBufferByteSize = vbByteSize;
	meshGeo->IndexFormat = DXGI_FORMAT_R32_UINT;
	meshGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	meshGeo->DrawArgs["BufferGeo"] = submesh;
	
	m_Meshs[meshName] = move(meshGeo);
	MaterialLoader->BuildMaterial(meshName, materials.front());
	

}


void MeshMgr::BuildModel(string meshName, MESH_TYPE etype)
{
	if (m_Meshs.count(meshName)) {
		cout << "중복 모델 로드" << endl;
		return;
	}

	// 메쉬가 없다면
	switch (etype)
	{
	case MESH_TYPE::ROCK:
		m_strFilePath = "..//Resources//Models//Rocks//";
		break;
	case MESH_TYPE::TREE:
		m_strFilePath = "..//Resources//Models//Trees//";
		break;
	case MESH_TYPE::HOUSE:
		m_strFilePath = "..//Resources//Models//Houses//";
		break;
	case MESH_TYPE::TILE:
		m_strFilePath = "..//Resources//Models//Tiles//";
	case MESH_TYPE::COUNT:
		break;
	default:
		break;
	}
	m_strFilePath += meshName + "//" + meshName;

	vector<Vertex>			Vertices;
	vector<UINT>			indices;
	vector<Material>		materials;


	bool check = true;
	check = LoadStaticMeshFile(Vertices, indices, &materials, m_strFilePath);
	if (!check) { cout << "메쉬 로드 실패" << endl; }


	const UINT vbByteSize = (UINT)Vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT);

	auto meshGeo = make_unique<MeshGeometry>();
	meshGeo->Name = "BufferGeo"; // 메시 이름

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &meshGeo->VertexBufferCPU));
	CopyMemory(meshGeo->VertexBufferCPU->GetBufferPointer(), Vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &meshGeo->IndexBufferCPU));
	CopyMemory(meshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	meshGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_pDevice,
		m_pCmdLst, Vertices.data(), vbByteSize, meshGeo->VertexBufferUploader);

	meshGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_pDevice,
		m_pCmdLst, indices.data(), ibByteSize, meshGeo->IndexBufferUploader);

	meshGeo->VertexByteStride = sizeof(Vertex);
	meshGeo->VertexBufferByteSize = vbByteSize;
	meshGeo->IndexFormat = DXGI_FORMAT_R32_UINT;
	meshGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	meshGeo->DrawArgs["BufferGeo"] = submesh;

	m_Meshs[meshName] = move(meshGeo);
	MaterialLoader->BuildMaterial(meshName, materials.front());
}

bool MeshMgr::LoadMeshFile(vector<SkinnedVertex>& outVertex, vector<UINT>& outIndex, vector<Material>* outMaterial, string path)
{
	path += ".mesh";
	ifstream fileInput(path);

	UINT vertexSize, indexSize;
	UINT materialSize;

	string ignore;
	if (fileInput) {
		fileInput >> ignore >> vertexSize;
		fileInput >> ignore >> indexSize;
		fileInput >> ignore >> materialSize;

		if (vertexSize == 0 || indexSize == 0)
			return false;
		
		fileInput >> ignore;
		for (UINT i = 0; i < materialSize; ++i) {
			// Material
			Material tempMaterial;
			// Fresnel, Emissive, Roughness 거르기
			XMFLOAT3 Fresnel_Net;
			XMFLOAT3 Emissive_Net;
			float Roughness_Net;

			fileInput >> ignore >> tempMaterial.Name;
			fileInput >> ignore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
			fileInput >> ignore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
			fileInput >> ignore >> Fresnel_Net.x >> Fresnel_Net.y >> Fresnel_Net.z;
			fileInput >> ignore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
			fileInput >> ignore >> Emissive_Net.x >> Emissive_Net.y >> Emissive_Net.z;
			fileInput >> ignore >> Roughness_Net;
			fileInput >> ignore;

			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					fileInput >> tempMaterial.MatTransform.m[j][k];
				}
			}
			(*outMaterial).push_back(tempMaterial);
		}

		for (UINT i = 0; i < vertexSize; ++i) {
			SkinnedVertex vertex; // Vertex
			int temp[4];

			fileInput >> ignore >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
			fileInput >> ignore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
			fileInput >> ignore >> vertex.TexC.x >> vertex.TexC.y;
			fileInput >> ignore >> vertex.Tangent.x >> vertex.Tangent.y >> vertex.Tangent.z;
			fileInput >> ignore >> vertex.Binormal.x >> vertex.Binormal.y >> vertex.Binormal.z;
			fileInput >> ignore >> vertex.BoneWeights.x >> vertex.BoneWeights.y >> vertex.BoneWeights.z;

			fileInput >> ignore >> temp[0] >> temp[1] >> temp[2] >> temp[3];

			for (int j = 0; j < 4; ++j)
			{
				vertex.BoneIndices[j] = temp[j];
			}
			outVertex.push_back(vertex);
		}

		fileInput >> ignore;
		for (UINT i = 0; i < indexSize; ++i) {
			UINT index;
			fileInput >> index;
			outIndex.push_back(index);
		}

		return true;

	}

	return false;
}

bool MeshMgr::LoadStaticMeshFile(vector<Vertex>& outVertex,
	vector<uint32_t>& outIndex, vector<Material>* outMaterial, string path)
{
	path += ".mesh";
	ifstream fileInput(path);

	UINT vertexSize, indexSize;
	UINT materialSize;

	string ignore;
	if (fileInput) {
		fileInput >> ignore >> vertexSize;
		fileInput >> ignore >> indexSize;
		fileInput >> ignore >> materialSize;

		if (vertexSize == 0 || indexSize == 0)
			return false;

		fileInput >> ignore;
		for (UINT i = 0; i < materialSize; ++i) {
			// Material
			Material tempMaterial;
			// Fresnel, Emissive, Roughness 거르기
			XMFLOAT3 Fresnel_Net;
			XMFLOAT3 Emissive_Net;
			float Roughness_Net;

			fileInput >> ignore >> tempMaterial.Name;
			fileInput >> ignore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
			fileInput >> ignore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
			fileInput >> ignore >> Fresnel_Net.x >> Fresnel_Net.y >> Fresnel_Net.z;
			fileInput >> ignore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
			fileInput >> ignore >> Emissive_Net.x >> Emissive_Net.y >> Emissive_Net.z;
			fileInput >> ignore >> Roughness_Net;
			fileInput >> ignore;

			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					fileInput >> tempMaterial.MatTransform.m[j][k];
				}
			}
			(*outMaterial).push_back(tempMaterial);
		}

		for (UINT i = 0; i < vertexSize; ++i) {
			Vertex vertex; // Vertex

			fileInput >> ignore >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
			fileInput >> ignore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
			fileInput >> ignore >> vertex.TexC.x >> vertex.TexC.y;
			fileInput >> ignore >> vertex.Tangent.x >> vertex.Tangent.y >> vertex.Tangent.z;
			fileInput >> ignore >> vertex.Binormal.x >> vertex.Binormal.y >> vertex.Binormal.z;
			
			outVertex.push_back(vertex);
		}

		fileInput >> ignore;
		for (UINT i = 0; i < indexSize; ++i) {
			UINT index;
			fileInput >> index;
			outIndex.push_back(index);
		}

		return true;

	}

	return false;
}

bool MeshMgr::LoadSkeletonFile(SkinnedData& outSkinnedData, string path)
{
	path += ".skeleton";
	std::ifstream fileIn(path);

	UINT boneSize;

	std::string ignore;
	if (fileIn)
	{
		fileIn >> ignore >> boneSize;

		if (boneSize == 0)
			return false;

		// Bone
		// Bone Hierarchy
		fileIn >> ignore;
		std::vector<int> boneHierarchy;
		for (UINT i = 0; i < boneSize; ++i)
		{
			int tempBoneHierarchy;
			fileIn >> tempBoneHierarchy;
			boneHierarchy.push_back(tempBoneHierarchy);
		}

		fileIn >> ignore;
		for (UINT i = 0; i < boneSize; ++i)
		{
			std::string tempBoneName;
			fileIn >> tempBoneName;

			outSkinnedData.SetBoneName(tempBoneName);
		}
		// Bone Offset
		fileIn >> ignore;
		std::vector<DirectX::XMFLOAT4X4> boneOffsets;
		for (UINT i = 0; i < boneSize; ++i)
		{
			DirectX::XMFLOAT4X4 tempBoneOffset;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					fileIn >> tempBoneOffset.m[i][j];
				}
			}
			boneOffsets.push_back(tempBoneOffset);
		}
		// Bone Submesh Offset
		fileIn >> ignore;
		std::vector<int> boneSubmeshOffset;
		for (UINT i = 0; i < boneSize; ++i)
		{
			int tempBoneSubmeshOffset;
			fileIn >> tempBoneSubmeshOffset;
			outSkinnedData.SetSubmeshOffset(tempBoneSubmeshOffset);
		}

		outSkinnedData.Set(
			boneHierarchy,
			boneOffsets);

		return true;
	}

	return false;
}
