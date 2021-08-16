#include "MeshMgr.h"
#include <fstream>
MeshMgr* MeshMgr::m_pInstance = nullptr;

void MeshMgr::InitMeshMgr()
{
}


void MeshMgr::BuildModel(string meshName)
{
	if (m_StaticMeshVtxInfo.count(meshName)) {
		cout << "중복 모델 로드" << endl;
		return;
	}

	m_strFilePath = "..//Resources//Models//";

	m_strFilePath += meshName;

	vector<Vertex>			Vertices;
	vector<UINT>			indices;
	vector<Material>		materials;


	bool check = true;
	check = LoadStaticMeshFile(Vertices, indices, &materials, m_strFilePath);
	if (!check) { cout << "메쉬 로드 실패" << endl; }

	m_StaticMeshVtxInfo[meshName].ulVertexCnt = Vertices.size();
	for (int i = 0; i < m_StaticMeshVtxInfo[meshName].ulVertexCnt; ++i)
		m_StaticMeshVtxInfo[meshName].vecVtxPos.push_back(Vertices[i].Pos);

	for (int i = 0; i < indices.size() / 3; ++i)
	{
		Indices indic;
		indic.i1 = indices[3 * i];
		indic.i2 = indices[3 * i + 1];
		indic.i3 = indices[3 * i + 2];
		m_StaticMeshVtxInfo[meshName].vecIdx.push_back(indic);
	}


}

void MeshMgr::BuildModels()
{
	BuildModel("Cylinder");
	BuildModel("barrel_group");
	BuildModel("box");
	BuildModel("castle_door_1");
	BuildModel("castle_tower_octagon");
	BuildModel("castle_tower_round");
	BuildModel("citadel_base_a");
	BuildModel("city_tower_a");
	BuildModel("civilian_house_18_a");
	BuildModel("civilian_house_18_b");
	BuildModel("civilian_house_36_c");
	BuildModel("civilian_house_37_e");
	BuildModel("civilian_house_37_h");
	BuildModel("civilian_house_46");
	BuildModel("civilian_house_ruin_e");
	BuildModel("fountain_a");
	BuildModel("overhang_building_a");
	BuildModel("roof_building_4");
	BuildModel("roof_building_5");
	BuildModel("roof_building_11");
	BuildModel("roof_building_12");
	BuildModel("street_gate");
	BuildModel("tower_gates");
	BuildModel("wall_H10m_L15m");
	BuildModel("wall_H10m_L50m");
	BuildModel("wagon");
	BuildModel("Sphere");

}

bool MeshMgr::LoadStaticMeshFile(vector<Vertex>& outVertex,
	vector<uint32_t>& outIndex, vector<Material>* outMaterial, string path)
{
	path += ".mesh";
	ifstream fileInput(path);

	UINT vertexSize, indexSize;
	UINT materialSize;

	//XMMATRIX xmmRotate = XMMatrixRotationX(XMConvertToRadians(-90.f));
	//xmmRotate *= XMMatrixRotationZ(XMConvertToRadians(-90.f));

	string ignore;
	if (fileInput) {
		fileInput >> ignore >> vertexSize;
		fileInput >> ignore >> indexSize;
		fileInput >> ignore >> materialSize;

		if (vertexSize == 0 || indexSize == 0)
			return false;
		fileInput >> ignore;
		if (materialSize != 0)
		{
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
		}

		for (UINT i = 0; i < vertexSize; ++i) {
			Vertex vertex; // Vertex

			fileInput >> ignore >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
			fileInput >> ignore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
			fileInput >> ignore >> vertex.TexC.x >> vertex.TexC.y;
			fileInput >> ignore >> vertex.Tangent.x >> vertex.Tangent.y >> vertex.Tangent.z;
			fileInput >> ignore >> vertex.Binormal.x >> vertex.Binormal.y >> vertex.Binormal.z;

			//XMStoreFloat3(&vertex.Pos, XMVector3TransformCoord(XMLoadFloat3(&vertex.Pos), xmmRotate));
			//XMStoreFloat3(&vertex.Normal, XMVector3TransformNormal(XMLoadFloat3(&vertex.Normal), xmmRotate));
			//XMStoreFloat3(&vertex.Tangent, XMVector3TransformNormal(XMLoadFloat3(&vertex.Tangent), xmmRotate));
			//XMStoreFloat3(&vertex.Binormal, XMVector3TransformNormal(XMLoadFloat3(&vertex.Binormal), xmmRotate));

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
