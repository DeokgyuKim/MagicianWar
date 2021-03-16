#pragma once
#include "framework.h"
#include "Geometry.h"
class Plane : public Geometry
{
public:
	Plane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int z);
	virtual ~Plane();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
	int m_fX, m_fZ;
};

