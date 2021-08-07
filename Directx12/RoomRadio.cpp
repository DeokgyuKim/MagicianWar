#include "RoomRadio.h"
#include "Renderer.h"
#include "TextController.h"
#include "Scene.h"

RoomRadio::RoomRadio(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, string strBaseTextureName, string strMouseOnTextureName, string strActiveTextureName, Scene* pScene, int iTag)
	: RadioButton(device, cmdLst, pRenderer, xmfInfo, strBaseTextureName, strMouseOnTextureName, strActiveTextureName)
{
    m_xmfRatio.x = 0.f;
    m_xmfRatio.y = 0.f;
	m_xmfInfo = xmfInfo;
	m_pScene = pScene;

	XMFLOAT4 info = XMFLOAT4(xmfInfo.x + 58.f, xmfInfo.y + 58.f, 64.f, 64.f);

	string roomname = "ROOM ";
	roomname += to_string(iTag);

	m_pTxtCtrl = new TextController(device, cmdLst, pRenderer, info, roomname.c_str(), pScene);
	m_pTxtCtrl->SetTextColor(XMFLOAT3(0.f, 1.f, 1.f));
}

RoomRadio::~RoomRadio()
{
	RemoveText();
}

void RoomRadio::LateUpdate(const float& fTimeDelta)
{
	Object::LateUpdate(fTimeDelta);
	m_pRenderer->PushObject(RENDER_TYPE::RENDER_UI_ROOMS, this);
	UiCB uicb;
	uicb.ratio = m_xmfRatio;
	m_UiCB->CopyData(0, uicb);
}

void RoomRadio::SetScrollPos(float Scorll, float fRatio)
{
	m_Rect.bottom = m_xmfInfo.y - Scorll + m_xmfInfo.w;
	m_Rect.top = m_xmfInfo.y - Scorll;
    m_xmfRatio.y = fRatio;
	if (m_pTxtCtrl != nullptr)
		m_pTxtCtrl->SetScrollPos(fRatio);
}

void RoomRadio::RemoveText()
{
	m_pTxtCtrl->RemoveTexts(m_pScene);
}
