#include "UI.h"
#include "Renderer.h"
#include "Core.h"

#include "Transform.h"
#include "UIPlane.h"

UI::UI(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strTextureName)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;

	Initialize(xmfInfo);
	m_strTextureName = strTextureName;
}

UI::~UI()
{
}

void UI::Initialize(XMFLOAT4 xmfInfo)
{
	Core::GetInstance()->CmdLstReset();
	Component* pComponent = new Transform(XMFLOAT3(1.f, 1.f, 1.f), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 0.f, 0.f));
	m_mapComponent["Transform"] = pComponent;

	pComponent = new UIPlane(m_pDevice, m_pCmdLst, m_pRenderer->GetHeap(), xmfInfo.x, xmfInfo.y, xmfInfo.z, xmfInfo.w);
	m_mapComponent["Mesh"] = pComponent;
	Core::GetInstance()->CmdLstExecute();
	Core::GetInstance()->WaitForGpuComplete();
}

int UI::Update(const float& fTimeDelta)
{
	Object::Update(fTimeDelta);
	return 0;
}

void UI::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);
	m_pRenderer->PushObject(RENDER_TYPE::RENDER_UI, this);
}

void UI::Render(const float& fTimeDelta, int _instanceCount)
{
	Object::Render(fTimeDelta);
}
