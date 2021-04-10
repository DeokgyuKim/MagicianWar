#include "Skybox.h"
#include "Renderer.h"
#include "Core.h"

#include "Camera.h"

#include "Transform.h"
#include "SkyBoxCube.h"

Skybox::Skybox(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer,
	string _meshName)
	:Object(_meshName)
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

int Skybox::Update(const float& fTimeDelta)
{
    return 0;
}

void Skybox::LateUpdate(const float& fTimeDelta)
{
	dynamic_cast<Transform*>(m_mapComponent["Transform"])->SetPosition(m_pCamera->GetPosition());
	Object::LateUpdate(fTimeDelta);

	m_pRenderer->PushObject(RENDER_TYPE::RENDER_SKYBOX, this);
}

void Skybox::Render(const float& fTimeDelta, int _instanceCount)
{
	Object::Render(fTimeDelta, _instanceCount);
}
