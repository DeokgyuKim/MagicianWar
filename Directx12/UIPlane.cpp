#include "UIPlane.h"

UIPlane::UIPlane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int y, int cx, int cy)
	: Geometry(device, cmdLst, heap)
{
	m_fX = (float)x; m_fY = (float)y;
	m_fCX = (float)cx; m_fCY = (float)cy;

	m_fX = ((m_fX - WINCX / 2) / WINCX) * 2;
	m_fY = ((m_fY / (WINCY / 2)) - 1) * -1;
	m_fCX = (m_fCX / WINCX) * 2;
	m_fCY = (m_fCY / WINCY) * 2;

	Initialize(device, heap);
}

UIPlane::~UIPlane()
{
}

HRESULT UIPlane::BuildGeometry(ID3D12Device* device)
{
	std::array<LightVertex, 4> vertices =
	{
		LightVertex({ XMFLOAT3(m_fX, m_fY, 0.f), XMFLOAT2(0.f, 0.f)}),
		LightVertex({ XMFLOAT3(m_fX + m_fCX, m_fY, 0.f), XMFLOAT2(1.f, 0.f)}),
		LightVertex({ XMFLOAT3(m_fX + m_fCX, m_fY - m_fCY, 0.f), XMFLOAT2(1.f, 1.f)}),
		LightVertex({ XMFLOAT3(m_fX, m_fY - m_fCY, 0.f), XMFLOAT2(0.f, 1.f)}),
	};

	std::array<std::uint16_t, 6> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(LightVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_BoxGeo = new MeshGeometry;
	m_BoxGeo->Name = "BufferGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		m_CmdLst.Get(), vertices.data(), vbByteSize, m_BoxGeo->VertexBufferUploader);

	m_BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		m_CmdLst.Get(), indices.data(), ibByteSize, m_BoxGeo->IndexBufferUploader);

	m_BoxGeo->VertexByteStride = sizeof(LightVertex);
	m_BoxGeo->VertexBufferByteSize = vbByteSize;
	m_BoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_BoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_BoxGeo->DrawArgs["BufferGeo"] = submesh;
	return S_OK;
}
