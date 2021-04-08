#include "StaticObject.h"
#include "Core.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

StaticObject::StaticObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, 
	XMFLOAT3 xmfPosition, XMFLOAT3 xmfRotate, XMFLOAT3 xmfScale, string MeshName)
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
	BuildConstantBuffer();

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
}

void StaticObject::Release()
{
}

HRESULT StaticObject::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
    return S_OK;
}

void StaticObject::UpdateObjectCB()
{
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix()));
	m_ObjectCB->CopyData(0, ObjCB);
}

int StaticObject::Update(const float& fTimeDelta)
{
	XMFLOAT3 rotate = dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetRotate();
	if (GetAsyncKeyState('I') & 0x8000)
	{
		rotate.x += 1.f;
		dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(rotate);
	}
	if (GetAsyncKeyState('O') & 0x8000)
	{
		rotate.y += 1.f;
		dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(rotate);
	}
	if (GetAsyncKeyState('P') & 0x8000)
	{
		rotate.z += 1.f;
		dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetRotate(rotate);
	}

	Object::Update(fTimeDelta);;
    
	return 0;
}

void StaticObject::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);

	// objCB Update
	UpdateObjectCB();

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_STATIC, this);
}

void StaticObject::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	
	Object::Render(fTimeDelta);
}
