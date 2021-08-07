#include "Text.h"
#include "Renderer.h"

Text::Text(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, XMFLOAT2 Idx)
	: UI(device, cmdLst, pRenderer, xmfInfo, "Ui_Text_AllText")
{
	m_xmfRatio = XMFLOAT2(0.f, 0.f);
	m_xmfScorllPos = Idx;
	m_TextCB = make_unique<UploadBuffer<TextCB>>(m_pDevice, 1, true);
}

Text::~Text()
{
}

void Text::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);
	m_pRenderer->PushObject(RENDER_TYPE::RENDER_UI_TEXT, this);
	UiCB uicb;
	uicb.ratio = m_xmfRatio;
	m_UiCB->CopyData(0, uicb);

	TextCB txtcb;
	txtcb.idx = m_xmfScorllPos;
	txtcb.txtcolor = m_xmfColor;
	m_TextCB->CopyData(0, txtcb);
}

void Text::Render(const float& fTimeDelta, int _instanceCount)
{
	m_pCmdLst->SetGraphicsRootConstantBufferView(20, m_UiCB->Resource()->GetGPUVirtualAddress());
	m_pCmdLst->SetGraphicsRootConstantBufferView(23, m_TextCB->Resource()->GetGPUVirtualAddress());
	Object::Render(fTimeDelta);
}
