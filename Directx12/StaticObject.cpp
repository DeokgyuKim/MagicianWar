#include "StaticObject.h"
#include "Core.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "InstanceMgr.h"

StaticObject::StaticObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
	XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName)
	:Object(MeshName)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	Initialize(xmfPosition, xmfRotate, xmfScale, MeshName);
}

StaticObject::~StaticObject()
{
}

void StaticObject::Initialize(XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName)
{
	Core::GetInstance()->CmdLstReset();


	Component* pComponent = new Transform(xmfScale, xmfRotate, xmfPosition);
	m_mapComponent["Transform"] = pComponent;
	pComponent = new Mesh(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), MeshName);
	m_mapComponent["Mesh"] = pComponent; // 이게 젤문제인듯
	pComponent = new MaterialCom(MeshName);
	m_mapComponent["Material"] = pComponent;

	m_strTextureName = "StaticTexturePow";
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetMeshRotate(XMFLOAT3(-90.f, 90.f, 0.f));
	m_MaterialIndex = dynamic_cast<MaterialCom*>(m_mapComponent["Material"])->GetMaterialIndex();


	///SetPhysX
	Object::LateUpdate(0.f);

#ifdef PHYSX
	CPhysXMgr::GetInstance()->CreateTriangleStaticMesh(this, MeshName, dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix());
#endif
}

void StaticObject::Release()
{
}



int StaticObject::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);;
    
	return 0;
}

void StaticObject::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_STATIC, this);
}

void StaticObject::Render(const float& fTimeDelta, int _instanceCount)
{
	Object::Render(fTimeDelta, _instanceCount);
}
