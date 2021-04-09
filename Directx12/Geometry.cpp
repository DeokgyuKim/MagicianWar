#include "Geometry.h"
#include "Renderer.h"
#include "DDSTexture.h"

Geometry::Geometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	m_CmdLst = cmdLst;
	m_InstanceCount = 1; // 인스턴스 설정안해주면 1개 그리는거겠지
}

Geometry::~Geometry()
{
}

void Geometry::Initialize(ID3D12Device* device, ID3D12DescriptorHeap* heap)
{
	
	BuildGeometry(device);
}

void Geometry::Render(const float& fTimeDelta, int _instanceCount)
{
	D3D12_VERTEX_BUFFER_VIEW vbv = m_BoxGeo->VertexBufferView();
	m_CmdLst->IASetVertexBuffers(0, 1, &vbv);
	D3D12_INDEX_BUFFER_VIEW ibv = m_BoxGeo->IndexBufferView();
	m_CmdLst->IASetIndexBuffer(&ibv);
	m_CmdLst->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_CmdLst->DrawIndexedInstanced( // 인스턴싱 구조에 맞지않는거같은데
		m_BoxGeo->DrawArgs["BufferGeo"].IndexCount,
		_instanceCount, 0, 0, 0);
}

