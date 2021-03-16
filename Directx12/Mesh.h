#pragma once
#include "Geometry.h"

class MeshMgr;
class SkinnedModelInstance;

class Mesh :
    public Geometry
{
public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst,
		ID3D12DescriptorHeap* heap,
		string strFilePath, MESH_TYPE etype
	);
	~Mesh();
public:
	//Get
	XMMATRIX    GetTexTransform() { return XMLoadFloat4x4(&TexTransform); }
	XMFLOAT4	GetDiffuseAlbedo() { return m_Material.DiffuseAlbedo; }
	XMFLOAT4	GetAmbient() { return XMFLOAT4(m_Material.Ambient.x, m_Material.Ambient.y, m_Material.Ambient.z, 1.f); }
	XMFLOAT4	GetSpecular() { return XMFLOAT4(m_Material.Specular.x, m_Material.Specular.y, m_Material.Specular.z, 1.f); }
	float		GetRoughness() { return m_Material.Roughness; }
	UINT		GetDiffuseMapIndex() { return m_Material.DiffuseSrvHeapIndex; }
	int			GetMatCBIndex() { return m_Material.MatCBIndex; }
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
private:
	string m_strFilePath;
	string meshName;
	MeshMgr* MeshLoader;
	// �ִϸ��̼� ����
	unique_ptr<SkinnedModelInstance> m_SkinnedModelInst;
	// Material Ŭ������ �Űܾ��ҵ�
	Material			 m_Material;
	// model
	// �ؽ�ó ��ǥ
	XMFLOAT4X4 TexTransform;

};

