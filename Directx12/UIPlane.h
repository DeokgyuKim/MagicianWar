#pragma once
#include "Geometry.h"
class UIPlane :
    public Geometry
{
public:
	UIPlane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int y, int cx, int cy);
	virtual ~UIPlane();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
private:
	float m_fX;
	float m_fY;
	float m_fCX;
	float m_fCY;
};

