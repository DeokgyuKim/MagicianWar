#include "Cube.h"
#include "Renderer.h"

Cube::Cube(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
	: Geometry(device, cmdLst, heap)
{
	m_CmdLst = cmdLst;
	Initialize(device, heap);

}

Cube::~Cube()
{
}


HRESULT Cube::BuildGeometry(ID3D12Device* device)
{
	std::array<ColorVertex, 8> vertices =
	{
		ColorVertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)	, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black)	, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red)		, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)	, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue)		, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)	, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan)		, XMFLOAT3(0.f, 1.f, 0.f) }),
		ColorVertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta)	, XMFLOAT3(0.f, 1.f, 0.f) })
	};

	XMFLOAT3 normal[8] = {	XMFLOAT3(-1.0f, -1.0f, -1.0f),
							XMFLOAT3(-1.0f, +1.0f, -1.0f),
							XMFLOAT3(+1.0f, +1.0f, -1.0f),
							XMFLOAT3(+1.0f, -1.0f, -1.0f),
							XMFLOAT3(-1.0f, -1.0f, +1.0f),
							XMFLOAT3(-1.0f, +1.0f, +1.0f),
							XMFLOAT3(+1.0f, +1.0f, +1.0f),
							XMFLOAT3(+1.0f, -1.0f, +1.0f) };

	for (int i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&normal[i], XMVector3Normalize(XMLoadFloat3(&normal[i])));
		vertices[i].Normal = normal[i];
	}



	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(ColorVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_BoxGeo = make_unique<MeshGeometry>();
	m_BoxGeo->Name = "BufferGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		m_CmdLst.Get(), vertices.data(), vbByteSize, m_BoxGeo->VertexBufferUploader);

	m_BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		m_CmdLst.Get(), indices.data(), ibByteSize, m_BoxGeo->IndexBufferUploader);

	m_BoxGeo->VertexByteStride = sizeof(ColorVertex);
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

