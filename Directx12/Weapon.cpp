#include "Weapon.h"
#include "Core.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "InstanceMgr.h"
#include "Animation.h"
#include "InstanceMgr.h"

Weapon::Weapon(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, string meshName, string textureName,
	Object* pParent, int socketIdx, XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPos)
	:Object(meshName)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;

	m_pParent = pParent;
	m_iSocketIdx = socketIdx;

	Initialize(meshName, textureName, xmfScale, xmfRotate, xmfPos);
}

Weapon::~Weapon()
{
}

void Weapon::Initialize(string MeshName, string textureName, XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPos)
{
	Core::GetInstance()->CmdLstReset();


	Component* pComponent = new Transform(xmfScale, xmfRotate, xmfPos);
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), MeshName);
	m_mapComponent["Mesh"] = pComponent;
	pComponent = new MaterialCom(MeshName);
	m_mapComponent["Material"] = pComponent;

	m_strTextureName = textureName;
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetParentMatrix(&m_xmf4x4ParentWorld);
}

void Weapon::Release()
{
}



int Weapon::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);
    
	return 0;
}

void Weapon::LateUpdate(const float& fTimeDelta)
{
	Component* Anim = m_pParent->GetComponent("Root_Animation");
	if (Anim != NULL)
	{
		// 21¹ø »À°¡ weapon
		XMStoreFloat4x4(&m_xmf4x4ParentWorld, 
			XMMatrixTranspose(XMLoadFloat4x4(&dynamic_cast<AnimationCom*>(Anim)->GetSkinnedModellnst()->FinalTransforms[m_iSocketIdx])) *
			XMLoadFloat4x4(dynamic_cast<Transform*>(m_pParent->GetTransController())->GetWorldMatrixPointer()));
	}

	Object::LateUpdate(fTimeDelta);

	InstanceMgr::GetInstnace()->UpdateInstanceBuffer(this);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_STATIC, this);
}

void Weapon::Render(const float& fTimeDelta, int _instanceCount)
{
	Object::Render(fTimeDelta, _instanceCount);
}
