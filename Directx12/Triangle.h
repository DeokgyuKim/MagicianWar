#pragma once
#include "framework.h"
#include "Object.h"
class Triangle : public Object
{
public:
	Triangle(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst);
	~Triangle();
public:
	void Initialize(ID3D12Device* device);
	virtual int		Update(const float& fTimeDelta);
	virtual void	LateUpdate(const float& fTimeDelta);
	virtual void	Render(const float& fTimeDelta);
private:
	HRESULT BuildDescriptorHeaps(ID3D12Device* device);
	HRESULT BuildConstantBuffers(ID3D12Device* device);
	HRESULT BuildBoxGeometry(ID3D12Device* device);

private:
	ComPtr<ID3D12GraphicsCommandList>	m_CmdLst;
	ComPtr<ID3D12DescriptorHeap>		m_CbvHeap;

	unique_ptr<MeshGeometry>			m_BoxGeo;
	unique_ptr<UploadBuffer<ObjectCB>>	m_ObjectCB;

	XMFLOAT3							m_xmfRotate;
	XMFLOAT4X4							m_xmmWorld;
	XMFLOAT3							m_xmfCamPos;
};

