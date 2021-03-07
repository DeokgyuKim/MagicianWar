#pragma once
#include "framework.h"
#include "Geometry.h"
class Cube : public Geometry
{
public:
	Cube(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	~Cube();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
};

