#pragma once
#include "framework.h"
#include "Buffer.h"
class Cube : public Buffer
{
public:
	Cube(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	~Cube();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
};

