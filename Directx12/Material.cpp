#include "Material.h"
#include "MaterialMgr.h"



MaterialCom::MaterialCom(string MaterialName)
{
	m_Material = MaterialMgr::GetInstnace()->GetMaterial(MaterialName);
	if (m_Material == NULL)
	{
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
}

MaterialCom::MaterialCom(const XMFLOAT4& diff, const XMFLOAT4& ambi, const XMFLOAT4& spec)
{
	m_xmfDiffuse = diff;
	m_xmfAmbient = ambi;
	m_xmfSpecular = spec;
}

MaterialCom::~MaterialCom()
{
	delete m_Material;
	m_Material = nullptr;
}
