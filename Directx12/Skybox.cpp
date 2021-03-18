#include "Skybox.h"
#include "Renderer.h"
#include "Core.h"

#include "Camera.h"

#include "Transform.h"
#include "SkyBoxCube.h"

Skybox::Skybox(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	Initialize();
}

Skybox::~Skybox()
{
}

void Skybox::Initialize()
{
	BuildConstantBuffer();

	Component* pComponent = new Transform(XMFLOAT3(100.f, 100.f, 100.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	Core::GetInstance()->CmdLstReset();
	pComponent = new SkyboxCube(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap());
	m_mapComponent["Mesh"] = pComponent;
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();

	m_strTextureName = "SkyBox";
}

void Skybox::Release()
{
}

HRESULT Skybox::BuildConstantBuffer()
{
	m_ObjectCB = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, 1, true);
    return S_OK;
}

int Skybox::Update(const float& fTimeDelta)
{
    return 0;
}

void Skybox::LateUpdate(const float& fTimeDelta)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(m_pCamera->GetPosition());
	Object::LateUpdate(fTimeDelta);

	// objCB Update
	ObjectCB	ObjCB;
	XMStoreFloat4x4(&ObjCB.World, XMMatrixTranspose(dynamic_cast<Transform*>(m_mapComponent["Transform"])->GetWorldMatrix()));
	m_ObjectCB->CopyData(0, ObjCB);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_SKYBOX, this);
}

void Skybox::Render(const float& fTimeDelta)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
	Object::Render(fTimeDelta);
}
