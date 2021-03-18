#pragma once
#include "framework.h"
#include "Geometry.h"
class SkyboxCube : public Geometry
{
public:
	SkyboxCube(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	virtual ~SkyboxCube();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
};

