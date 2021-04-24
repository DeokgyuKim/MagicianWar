#pragma once
#include "framework.h"
#include "Geometry.h"
class Plane : public Geometry
{
public:
	Plane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int z, bool bNormalY = true);
	virtual ~Plane();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
	bool m_bNormalY;
	int m_fX, m_fZ;
};

