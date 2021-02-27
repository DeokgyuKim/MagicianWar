#include "Buffer.h"
#include "Renderer.h"
#include "DDSTexture.h"

Buffer::Buffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	m_CmdLst = cmdLst;
}

Buffer::~Buffer()
{
}

void Buffer::Initialize(ID3D12Device* device, ID3D12DescriptorHeap* heap)
{
	BuildGeometry(device);
}

void Buffer::Render(const float& fTimeDelta)
{
	D3D12_VERTEX_BUFFER_VIEW vbv = m_BoxGeo->VertexBufferView();
	m_CmdLst->IASetVertexBuffers(0, 1, &vbv);
	D3D12_INDEX_BUFFER_VIEW ibv = m_BoxGeo->IndexBufferView();
	m_CmdLst->IASetIndexBuffer(&ibv);
	m_CmdLst->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_CmdLst->DrawIndexedInstanced(
		m_BoxGeo->DrawArgs["BufferGeo"].IndexCount,
		1, 0, 0, 0);
}

