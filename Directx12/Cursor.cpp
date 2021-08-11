#include "Cursor.h"
#include "Renderer.h"

Cursor::Cursor(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer)
	: UI(device, cmdLst, pRenderer, XMFLOAT4(0.f, 0.f, 40.f, 40.f), "Cursor")
{
}

Cursor::~Cursor()
{
}

int Cursor::Update(const float& fTimeDelta)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	m_xmfRatio.x = (pt.x * 2.f) / WINCX;
	m_xmfRatio.y = -(pt.y * 2.f) / WINCY;

	return 0;
}

void Cursor::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);
	m_pRenderer->PushObject(RENDER_TYPE::RENDER_CURSOR, this);
	UiCB uicb;
	uicb.ratio = m_xmfRatio;
	m_UiCB->CopyData(0, uicb);
}
