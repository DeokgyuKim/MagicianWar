#include "Player.h"

#include "Renderer.h"
#include "Geometry.h"
#include "Cube.h"
#include "Mesh.h"
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
	BuildConstantBuffer();

	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), CHARACTER_WIZARD_01, MESH_TYPE::CHARACTER);
	m_mapComponent["Mesh"] = pComponent;
}

void Player::Release()
{
}

HRESULT Player::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_SkinnedCB = make_unique<UploadBuffer<SkinnedCB>>(m_pDevice, 1, true);
	m_MaterialCB = make_unique<UploadBuffer<MaterialCB>>(m_pDevice, 1, true);
	//UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	//D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();
	//// Offset to the ith object constant buffer in the buffer.
	//int boxCBufIndex = 0;
	//cbAddress += boxCBufIndex * objCBByteSize;

	//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	//cbvDesc.BufferLocation = cbAddress;
	//cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectCB));

	//m_pRenderer->CreateConstantBufferView(cbvDesc);
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

	// objCB Update
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix()));
	XMStoreFloat4x4(&ObjCB.TexTransform, XMMatrixTranspose(dynamic_cast<Mesh*>(m_mapComponent["Mesh"])->GetTexTransform()));
	ObjCB.MaterialIndex = 0;
	m_ObjectCB->CopyData(0, ObjCB);

	// SkinnedCB // 이거 애니메이션 붙이기 전까지 붙이면 안뜸..
	
	// MaterialCB
	
	MaterialCB matCB;
	matCB.DiffuseAlbedo = dynamic_cast<Mesh*>(m_mapComponent["Mesh"])->GetDiffuseAlbedo();
	matCB.Roughness = dynamic_cast<Mesh*>(m_mapComponent["Mesh"])->GetRoughness();
	matCB.DiffuseMapIndex = dynamic_cast<Mesh*>(m_mapComponent["Mesh"])->GetDiffuseMapIndex();
	m_MaterialCB->CopyData(0, matCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_MOVABLE, this);

}

void Player::Render(const float& fTimeDelta)
{

	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootShaderResourceView(5, m_MaterialCB->Resource()->GetGPUVirtualAddress());

	Object::Render(fTimeDelta);
	//m_pBuffer->Render(fTimeDelta);
}

XMFLOAT3 Player::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

void Player::UpdateSkinnedAnimation(const float fTimeDelta)
{
}
