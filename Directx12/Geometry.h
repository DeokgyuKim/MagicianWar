#pragma once
#include "framework.h"
#include "Component.h"

class Geometry : public Component
{
public:
	Geometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	~Geometry();
public:
	void			Initialize(ID3D12Device* device, ID3D12DescriptorHeap* heap);
	virtual void	Render(const float& fTimeDelta);
private:
	virtual HRESULT BuildGeometry(ID3D12Device* device) PURE;
	
protected:
	ComPtr<ID3D12GraphicsCommandList>	m_CmdLst;
	unique_ptr<MeshGeometry>			m_BoxGeo;
};

