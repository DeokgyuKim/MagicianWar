#include "Material.h"

MaterialCom::MaterialCom(const XMFLOAT4& diff, const XMFLOAT4& ambi, const XMFLOAT4& spec)
{
	m_xmfDiffuse = diff;
	m_xmfAmbient = ambi;
	m_xmfSpecular = spec;
}

MaterialCom::~MaterialCom()
{
}
