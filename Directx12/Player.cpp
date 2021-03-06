#include "Player.h"

#include "Renderer.h"
#include "Buffer.h"
#include "Cube.h"
#include "Camera.h"
#include "Transform.h"

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

	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
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
	Object::Update(fTimeDelta);;

	if (m_pCamera->GetMode() == CAMERA_MODE::CAMERA_THIRD)
	{
		XMFLOAT3 xmfRotate = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();
		xmfRotate.y = m_pCamera->GetRotY();
		dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(xmfRotate);

		dynamic_cast<Transform*>(m_mapComponent["Transform"])->KeyInput();
	}


	return 0;
}

void Player::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix()));

	m_ObjectCB->CopyData(0, ObjCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_COLOR, this);

}

void Player::Render(const float& fTimeDelta)
{
	Object::Render(fTimeDelta);;

	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());

	m_pBuffer->Render(fTimeDelta);
}

XMFLOAT3 Player::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}
