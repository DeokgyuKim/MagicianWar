#include "MaterialMgr.h"


MaterialMgr* MaterialMgr::m_pInstance = nullptr;

void MaterialMgr::InitMaterialMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst)
{
	m_pCore = pCore;
	m_pDevice = pDevice;
	m_pCmdLst = pCmdLst;
}

void MaterialMgr::BuildMaterial(const string& MaterialName, Material Mat)
{
	unique_ptr<Material> _material = make_unique<Material>();
	_material->Name = Mat.Name;
	_material->Ambient = Mat.Ambient;
	_material->DiffuseAlbedo = Mat.DiffuseAlbedo;
	_material->Specular = Mat.Specular;
	_material->MatTransform = Mat.MatTransform;

	m_Materials[MaterialName] = move(_material);

}

Material* MaterialMgr::GetMaterial(const string& MaterialName)
{
	auto iter = m_Materials.find(MaterialName);
	if (iter == m_Materials.end())
		return nullptr;
	return (*iter).second.get();
}

