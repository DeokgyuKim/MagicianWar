#include "Plane.h"
#include "Renderer.h"
#include "DDSTexture.h"

Plane::Plane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, int x, int z, bool bNormalY)
	:Geometry(device, cmdLst, heap)
{
	m_fX = (float)x; m_fZ = (float)z;
	m_bNormalY = bNormalY;
	Initialize(device, heap);
}

Plane::~Plane()
{
}

HRESULT Plane::BuildGeometry(ID3D12Device* device)
{
	std::array<UvVertex, 4> vertices;
	if (m_bNormalY)
	{
		vertices =
		{
			UvVertex({ XMFLOAT3(m_fX - 0.5f,	0.f,	m_fZ + 0.5f), XMFLOAT2(0.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f) }),
			UvVertex({ XMFLOAT3(m_fX + 0.5f,	0.f,	m_fZ + 0.5f), XMFLOAT2(1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f) }),
			UvVertex({ XMFLOAT3(m_fX + 0.5f,	0.f,	m_fZ - 0.5f), XMFLOAT2(1.f, 1.f), XMFLOAT3(0.f, 1.f, 0.f) }),
			UvVertex({ XMFLOAT3(m_fX - 0.5f,	0.f,	m_fZ - 0.5f), XMFLOAT2(0.f, 1.f), XMFLOAT3(0.f, 1.f, 0.f) }),
		};
	}
	else
	{
		vertices =
		{
			UvVertex({ XMFLOAT3(0.f,	m_fX + 0.5f,	m_fZ - 0.5f), XMFLOAT2(0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f) }),
			UvVertex({ XMFLOAT3(0.f,	m_fX + 0.5f,	m_fZ + 0.5f), XMFLOAT2(1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f) }),
			UvVertex({ XMFLOAT3(0.f,	m_fX - 0.5f,	m_fZ + 0.5f), XMFLOAT2(1.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f) }),
			UvVertex({ XMFLOAT3(0.f,	m_fX - 0.5f,	m_fZ - 0.5f), XMFLOAT2(0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f) }),
		};
	}

	std::array<std::uint16_t, 6> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(UvVertex);
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

	m_BoxGeo->VertexByteStride = sizeof(UvVertex);
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

