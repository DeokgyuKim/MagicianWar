#include "Plane.h"
#include "Renderer.h"
#include "DDSTexture.h"

Plane::Plane(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst)
{
	m_CmdLst = cmdLst;
	m_pRenderer = Renderer::GetInstance();
	Initialize(device);

	m_xmfRotate = {};
	m_xmmWorld = MathHelper::Identity4x4();

	m_xmfCamPos.x = 0.f;
	m_xmfCamPos.y = 0.f;
	m_xmfCamPos.z = 0.f;

}

Plane::~Plane()
{
}

void Plane::Initialize(ID3D12Device* device)
{
	//BuildDescriptorHeaps(device);
	BuildConstantBuffers(device);
	BuildBoxGeometry(device);

}

int Plane::Update(const float& fTimeDelta)
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_xmfRotate.y -= 1.f;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_xmfRotate.y += 1.f;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_xmfCamPos.z += 1.f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_xmfCamPos.z -= 1.f;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_xmfCamPos.x -= 1.f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_xmfCamPos.x += 1.f;
	}
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		m_xmfCamPos.y -= 1.f;
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_xmfCamPos.y += 1.f;
	}
	return 0;
}

void Plane::LateUpdate(const float& fTimeDelta)
{
	XMStoreFloat4x4(&m_xmmWorld, XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y)));

	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 800.f / 600.f, 1.0f, 1000.0f);

	XMVECTOR pos = XMVectorSet(0.f, 3.f, -3.f, 1.0f);
	XMVECTOR target = XMVectorSet(m_xmfCamPos.x, m_xmfCamPos.y, m_xmfCamPos.z, 1.0f);//XMVectorZero(); 
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

	XMMATRIX world = XMLoadFloat4x4(&m_xmmWorld) * view * proj;
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(world));
	m_ObjectCB->CopyData(0, ObjCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_NOBLEND, this);
}

void Plane::Render(const float& fTimeDelta)
{
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_pRenderer->m_pTexture->m_ptrSRVHeap.Get() };
	m_CmdLst->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_pRenderer->m_pTexture->m_ptrSRVHeap->GetGPUDescriptorHandleForHeapStart());

	m_CmdLst->SetGraphicsRootDescriptorTable(1, tex);

	D3D12_VERTEX_BUFFER_VIEW vbv = m_BoxGeo->VertexBufferView();
	m_CmdLst->IASetVertexBuffers(0, 1, &vbv);
	D3D12_INDEX_BUFFER_VIEW ibv = m_BoxGeo->IndexBufferView();
	m_CmdLst->IASetIndexBuffer(&ibv);
	m_CmdLst->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_CmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());

	m_CmdLst->DrawIndexedInstanced(
		m_BoxGeo->DrawArgs["PlaneGeo"].IndexCount,
		1, 0, 0, 0);
}

HRESULT Plane::BuildDescriptorHeaps(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_CbvHeap));
	return S_OK;
}

HRESULT Plane::BuildConstantBuffers(ID3D12Device* device)
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(device, 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRenderer->m_pTexture->m_ptrSRVHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += gnCbvSrvUavDescriptorIncrementSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	device->CreateConstantBufferView(
		&cbvDesc,
		handle);
	return S_OK;
}


HRESULT Plane::BuildBoxGeometry(ID3D12Device* device)
{
	std::array<UvVertex, 4> vertices =
	{
		UvVertex({ XMFLOAT3(-1.0f,	0.0f,	1.0f), XMFLOAT2(0.f, 0.f) }),
		UvVertex({ XMFLOAT3(1.0f,	0.0f,	1.0f), XMFLOAT2(1.f, 0.f) }),
		UvVertex({ XMFLOAT3(1.0f,	0.0f,	-1.0f), XMFLOAT2(1.f, 1.f) }),
		UvVertex({ XMFLOAT3(-1.0f,	0.0f,	-1.0f), XMFLOAT2(0.f, 1.f) }),
	};

	std::array<std::uint16_t, 6> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(UvVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_BoxGeo = make_unique<MeshGeometry>();
	m_BoxGeo->Name = "PlaneGeo";

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

	m_BoxGeo->DrawArgs["PlaneGeo"] = submesh;
	return S_OK;
}

