#pragma once
#include "Component.h"

class MaterialCom :
    public Component
{
public:
    MaterialCom(string MaterialName);
    MaterialCom(string MaterialName,const XMFLOAT4& diff, const XMFLOAT4& ambi, const XMFLOAT4& spec);
    virtual ~MaterialCom();
public:
    void    SetDiffuse(XMFLOAT4 diff) { m_xmfDiffuse = diff; }
    void    SetAmbient(XMFLOAT4 ambi) { m_xmfAmbient = ambi; }
    void    SetSpecular(XMFLOAT4 spec) { m_xmfSpecular = spec; }
public:
    XMVECTOR    GetDiffuse() { return XMLoadFloat4(&m_xmfDiffuse); }
    XMVECTOR    GetAmbient() { return XMLoadFloat4(&m_xmfAmbient); }
    XMVECTOR    GetSpecular() { return XMLoadFloat4(&m_xmfSpecular); }
    int         GetMaterialIndex() { return MaterialIndex; }
private:
    Material* m_Material;
    XMFLOAT4    m_xmfDiffuse;
    XMFLOAT4    m_xmfAmbient;
    XMFLOAT4    m_xmfSpecular;

    int MaterialIndex;

};
