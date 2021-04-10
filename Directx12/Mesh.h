#pragma once
#include "Geometry.h"

class MeshMgr;

class Mesh :
    public Geometry
{
public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst,
		ID3D12DescriptorHeap* heap,
		string _meshName
	);
	~Mesh();
public:
	//Get
	XMMATRIX    GetTexTransform() { return XMLoadFloat4x4(&TexTransform); }

	// Material Data
	XMFLOAT4	GetDiffuseAlbedo() { return m_Material.DiffuseAlbedo; }
	XMFLOAT4	GetAmbient() { return XMFLOAT4(m_Material.Ambient.x, m_Material.Ambient.y, m_Material.Ambient.z, 1.f); }
	XMFLOAT4	GetSpecular() { return XMFLOAT4(m_Material.Specular.x, m_Material.Specular.y, m_Material.Specular.z, 1.f); }

private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
private:
	
	string meshName;
	string InstanceID;
	MeshMgr* MeshLoader;
	// Material 클래스로 옮겨야할듯
	Material			 m_Material;

	
	// model
	// 텍스처 좌표
	XMFLOAT4X4 TexTransform;

};

