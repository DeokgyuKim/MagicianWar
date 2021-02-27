#pragma once
#include "framework.h"
#include "Buffer.h"
class Plane : public Buffer
{
public:
	Plane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int z);
	~Plane();
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device);
	int m_fX, m_fZ;
};

