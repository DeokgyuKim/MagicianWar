#include "MeshMgr.h"
#include <fstream>
MeshMgr* MeshMgr::m_pInstance = nullptr;

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
			// FresnelÀÌ¶û Emissive °Å¸£±â
			XMFLOAT3 Fresnel_Net;
			XMFLOAT3 Emissive_Net;

			fileInput >> ignore >> tempMaterial.Name;
			fileInput >> ignore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
			fileInput >> ignore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
			fileInput >> ignore >> Fresnel_Net.x >> Fresnel_Net.y >> Fresnel_Net.z;
			fileInput >> ignore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
			fileInput >> ignore >> Emissive_Net.x >> Emissive_Net.y >> Emissive_Net.z;
			fileInput >> ignore >> tempMaterial.Roughness;
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
