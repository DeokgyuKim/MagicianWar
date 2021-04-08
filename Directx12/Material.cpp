#include "Material.h"
#include "MaterialMgr.h"



MaterialCom::MaterialCom(string MaterialName)
{
	MaterialIndex = 0;
	m_Material = MaterialMgr::GetInstnace()->GetMaterial(MaterialName);
	if (m_Material == NULL)
	{
		Material MaterialData;
		MaterialData.Ambient = XMFLOAT3(0.588f, 0.588f, 0.588f);
		MaterialData.DiffuseAlbedo = XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f);
		MaterialData.Specular = XMFLOAT3(1.f, 1.f, 1.f);
		MaterialData.Name = MaterialName;
		MaterialMgr::GetInstnace()->BuildMaterial(MaterialName, MaterialData);

		m_xmfDiffuse = XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f);
		m_xmfAmbient = XMFLOAT4(0.588f, 0.588f, 0.588f, 1.f);
		m_xmfSpecular = XMFLOAT4(1.f, 1.f, 1.f, 1.f);

		
	}
	else
	{
		m_xmfDiffuse = m_Material->DiffuseAlbedo;
		m_xmfAmbient = XMFLOAT4(m_Material->Ambient.x, m_Material->Ambient.y, m_Material->Ambient.z, 1.0);
		m_xmfSpecular = XMFLOAT4(m_Material->Specular.x, m_Material->Specular.y, m_Material->Specular.z, 1.0);
		
	}
	m_xmfDiffuse = XMFLOAT4(0.633f, 0.833f, 0.833f, 1.f);
	m_xmfAmbient = XMFLOAT4(0.243f, 0.243f, 0.243f, 1.f);

	MaterialIndex = MaterialMgr::GetInstnace()->GetMaterial(MaterialName)->MaterialIndex;
}

MaterialCom::MaterialCom(string MaterialName, 
	const XMFLOAT4& diff, const XMFLOAT4& ambi, const XMFLOAT4& spec)
{
	Material MaterialData;
	MaterialData.Ambient = XMFLOAT3(ambi.x, ambi.y, ambi.z);
	MaterialData.DiffuseAlbedo = diff;
	MaterialData.Specular = XMFLOAT3(spec.x, spec.y, spec.z);
	MaterialData.Name = MaterialName;
	MaterialMgr::GetInstnace()->BuildMaterial(MaterialName, MaterialData);

	m_xmfDiffuse = diff;
	m_xmfAmbient = ambi;
	m_xmfSpecular = spec;

	MaterialIndex = MaterialMgr::GetInstnace()->GetMaterial(MaterialName)->MaterialIndex;
}

MaterialCom::~MaterialCom()
{
	delete m_Material;
	m_Material = nullptr;
}
