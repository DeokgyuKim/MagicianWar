#include "MaterialMgr.h"


MaterialMgr* MaterialMgr::m_pInstance = nullptr;


void MaterialMgr::UpdateMaterialCBs()
{
	if (m_MaterialCBs == NULL)
		return;

	auto curMaterialCBs = m_MaterialCBs.get();

	for (auto& e : m_Materials) // 현재 존재하는 Material 갯수만큼
	{
		Material* mat = e.second.get();

		MaterialCB data;
		data.Diffuse = mat->DiffuseAlbedo;
		data.Ambient = XMFLOAT4(mat->Ambient.x, mat->Ambient.y, mat->Ambient.z, 1.f);
		data.Specular = XMFLOAT4(mat->Specular.x, mat->Specular.y, mat->Specular.z, 1.f);

		curMaterialCBs->CopyData(mat->MaterialIndex, data);
	}


}

void MaterialMgr::InitMaterialMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst)
{
	m_pCore = pCore;
	m_pDevice = pDevice;
	m_pCmdLst = pCmdLst;

}

void MaterialMgr::BuildMaterial(const string& MaterialName, Material Mat)
{

	if (m_Materials.count(MaterialName)) {
		cout << "동일 Material 존재" << endl;
		return;
	}


	unique_ptr<Material> _material = make_unique<Material>();
	_material->Name = MaterialName;
	_material->Ambient = Mat.Ambient;
	_material->DiffuseAlbedo = Mat.DiffuseAlbedo;
	_material->Specular = Mat.Specular;
	_material->MatTransform = Mat.MatTransform;
	_material->MaterialIndex = ++MaterialCount; // 0 1 2 Material 이름에 해당하는 Index 저장 
	m_Materials[MaterialName] = move(_material);

}

void MaterialMgr::BuildMaterialCBs()
{
	m_MaterialCBs = make_unique<UploadBuffer<MaterialCB>>(m_pDevice, m_Materials.size(), false);
}

void MaterialMgr::SetGraphicsShaderResourceView()
{
	if (m_MaterialCBs == NULL)
		return;
	auto MatBuffer = m_MaterialCBs->Resource();
	m_pCmdLst->SetGraphicsRootShaderResourceView(2, MatBuffer->GetGPUVirtualAddress());
}

Material* MaterialMgr::GetMaterial(const string& MaterialName)
{
	auto iter = m_Materials.find(MaterialName);
	if (iter == m_Materials.end())
		return nullptr;
	return (*iter).second.get();
}

