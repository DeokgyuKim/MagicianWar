#pragma once
#include "Geometry.h"
class LightGeo :
    public Geometry
{
public:
	LightGeo(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int y);
	virtual ~LightGeo();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
private:
	float m_fX;
	float m_fY;
};

