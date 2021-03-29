#include "Bullet.h"

#include "Renderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "Core.h"

 // Component
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Animation.h"

Bullet::Bullet(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
	XMFLOAT3 Pos, XMFLOAT3 Rotate, float speed)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	m_fSpeed = speed;

	Initialize(Pos, Rotate);

	
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(XMFLOAT3 Pos, XMFLOAT3 Rotate)
{
	Core::GetInstance()->CmdLstReset();

	BuildConstantBuffer();
	Component* pComponent = new Transform(XMFLOAT3(0.1f, 0.1f, 0.1f), Rotate, Pos);
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Sphere");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("Sphere");
	m_mapComponent["Material"] = pComponent;

	m_strTextureName = "FireBall3";

	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
}

void Bullet::Release()
{
}

HRESULT Bullet::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
	m_MaterialCB = make_unique<UploadBuffer<MaterialCB>>(m_pDevice, 1, true);

	return S_OK;
}

int Bullet::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->MoveForward(fTimeDelta * m_fSpeed);

	return 0;
}

void Bullet::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	// objCB Update
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix()));
	m_ObjectCB->CopyData(0, ObjCB);

	// MaterialCB
	MaterialCB matCB;
	XMStoreFloat4(&matCB.Diffuse, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetDiffuse());
	XMStoreFloat4(&matCB.Ambient, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetAmbient());
	XMStoreFloat4(&matCB.Specular, dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetSpecular());
	m_MaterialCB->CopyData(0, matCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_BULLET, this);

}

void Bullet::Render(const float& fTimeDelta)
{

	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(2, m_MaterialCB->Resource()->GetGPUVirtualAddress());
	Object::Render(fTimeDelta);
	//m_pBuffer->Render(fTimeDelta);
}

XMFLOAT3 Bullet::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

void Bullet::UpdateSkinnedAnimation(const float fTimeDelta)
{
}
