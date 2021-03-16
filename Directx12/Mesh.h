#pragma once
#include "Geometry.h"
class Mesh :
    public Geometry
{
public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, string strFilePath);
	virtual ~Mesh();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
private:
	string m_strFilePath;
};

