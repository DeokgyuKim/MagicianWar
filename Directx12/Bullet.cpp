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

Bullet::Bullet(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string _meshName,
	XMFLOAT3 Pos, XMFLOAT3 Rotate, float speed)
	:Object(_meshName)
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


	Component* pComponent = new Transform(XMFLOAT3(0.1f, 0.1f, 0.1f), Rotate, Pos);
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), "Sphere");
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom("Sphere");
	m_mapComponent["Material"] = pComponent;

	m_strTextureName = "FireBall3";
	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
}

void Bullet::Release()
{
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

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_BULLET, this);

}

void Bullet::Render(const float& fTimeDelta, int _instanceCount)
{

	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	Object::Render(fTimeDelta, _instanceCount);

}



XMFLOAT3 Bullet::GetPosition()
{
	return dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetPosition();
}

void Bullet::UpdateSkinnedAnimation(const float fTimeDelta)
{
}
