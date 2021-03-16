#include "Player.h"

#include "Renderer.h"
#include "Geometry.h"
#include "Cube.h"
#include "Camera.h"
#include "Transform.h"
#include "Material.h"

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
	BuildConstantBuffer();

	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Cube(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap());
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom(XMFLOAT4(1.f, 1.f, 1.f, 1.f), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.f), XMFLOAT4(0.f, 0.1f, 0.1f, 1.f));
	m_mapComponent["Material"] = pComponent;
}

void Player::Release()
{
}

HRESULT Player::BuildConstantBuffer()
{
	///World
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


	///Diffuse
	m_DiffuseCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_DiffuseCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);

	///Ambient
	m_AmbientCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_AmbientCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	m_pRenderer->CreateConstantBufferView(cbvDesc);

	///Specular
	m_SpecularCB = make_unique<UploadBuffer<MatCB>>(m_pDevice, 1, true);

	objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

	cbAddress = m_SpecularCB->Resource()->GetGPUVirtualAddress();
	cbAddress += boxCBufIndex * objCBByteSize;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(MatCB));

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

	MatCB		DiffCB;
	XMStoreFloat4(&DiffCB.Material, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetDiffuse());
	m_DiffuseCB->CopyData(0, DiffCB);

	MatCB		AmbiCB;
	XMStoreFloat4(&AmbiCB.Material, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetAmbient());
	m_AmbientCB->CopyData(0, AmbiCB);

	MatCB		SpecCB;
	XMStoreFloat4(&SpecCB.Material, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetSpecular());
	m_SpecularCB->CopyData(0, SpecCB);


	m_pRenderer->PushObject(RENDER_TYPE::RENDER_COLOR, this);

}

void Player::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(3, m_DiffuseCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(4, m_AmbientCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(5, m_SpecularCB->Resource()->GetGPUVirtualAddress());

	Object::Render(fTimeDelta);
	//m_pBuffer->Render(fTimeDelta);
}

XMFLOAT3 Player::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}
