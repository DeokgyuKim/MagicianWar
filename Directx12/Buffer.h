#pragma once
#include "framework.h"
class Buffer
{
public:
	Buffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	~Buffer();
public:
	void	Initialize(ID3D12Device* device, ID3D12DescriptorHeap* heap);
	void	Render(const float& fTimeDelta);
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device) PURE;

protected:
	ComPtr<ID3D12GraphicsCommandList>	m_CmdLst;

	unique_ptr<MeshGeometry>			m_BoxGeo;
};

