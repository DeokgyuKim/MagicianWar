#include "Player.h"

#include "Renderer.h"
#include "Buffer.h"
#include "Cube.h"
#include "Camera.h"

Player::Player(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	Initialize();

	
}

Player::~Player()
{
}

void Player::Initialize()
{
	m_pBuffer = new Cube(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap());
	BuildConstantBuffer();

	m_xmfScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_xmfRotate = XMFLOAT3(0.f, 0.f, 0.f);
	m_xmfPosition = XMFLOAT3(0.f, 0.f, 0.f);

	XMStoreFloat4x4(&m_xmmWorld, XMMatrixIdentity());
}

void Player::Release()
{
	if (m_pBuffer != NULL)
		delete m_pBuffer;
	m_pBuffer = NULL;
}

HRESULT Player::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);
	return S_OK;
}

int Player::Update(const float& fTimeDelta)
{
	if (m_pCamera->GetMode() == CAMERA_MODE::CAMERA_THIRD)
	{
		XMFLOAT3 look, right;
		memcpy(&look, &m_xmmWorld._31, sizeof(XMFLOAT3));
		memcpy(&right, &m_xmmWorld._11, sizeof(XMFLOAT3));

		m_xmfRotate.y = m_pCamera->GetRotY();

		if (GetAsyncKeyState('W') & 0x8000)
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&look) * 0.1f);
		if (GetAsyncKeyState('S') & 0x8000)
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&look) * 0.1f);
		if (GetAsyncKeyState('A') & 0x8000)
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) - XMLoadFloat3(&right) * 0.1f);
		if (GetAsyncKeyState('D') & 0x8000)
			XMStoreFloat3(&m_xmfPosition, XMLoadFloat3(&m_xmfPosition) + XMLoadFloat3(&right) * 0.1f);
	}


	return 0;
}

void Player::LateUpdate(const float& fTimeDelta)
{
	XMMATRIX scale, rotateX, rotateY, rotateZ, transform;

	scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_xmfScale));
	
	rotateX = XMMatrixRotationX(XMConvertToRadians(m_xmfRotate.x));
	rotateY = XMMatrixRotationY(XMConvertToRadians(m_xmfRotate.y));
	rotateZ = XMMatrixRotationZ(XMConvertToRadians(m_xmfRotate.z));

	transform = XMMatrixTranslationFromVector(XMLoadFloat3(&m_xmfPosition));

	XMMATRIX world = scale * rotateX * rotateY * rotateZ * transform;// *view* proj;

	XMStoreFloat4x4(&m_xmmWorld, world);

	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(world));


	m_ObjectCB->CopyData(0, ObjCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_COLOR, this);
}

void Player::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());

	m_pBuffer->Render(fTimeDelta);
}
