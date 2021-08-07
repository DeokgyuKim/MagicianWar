#include "RoomRadioController.h"
#include "RoomRadio.h"

#include "Core.h"
#include "Renderer.h"
#include "Scene.h"

RoomRadioController::RoomRadioController(XMFLOAT4 xmfBox)
	: RadioButtonController()
{
	m_Rect.left = xmfBox.x;
	m_Rect.bottom = xmfBox.y + xmfBox.w;
	m_Rect.right = xmfBox.x + xmfBox.z;
	m_Rect.top = xmfBox.y;
}

RoomRadioController::~RoomRadioController()
{
}

void RoomRadioController::SetScrollPos(int Zdir, float speed)
{
	if (m_vecRadios.size() < 4)
	{
		for (auto Button : m_vecRadios)
			dynamic_cast<RoomRadio*>(Button)->SetScrollPos(0.f, 0.f);
		return;
	}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (!PtInRect(&m_Rect, pt))
		return;

	float Scroll = Zdir * speed * 0.5f;
	m_fScrollPos += Scroll;

	m_fScrollPos = max(m_fScrollPos, 0.f);
	m_fScrollPos = min(m_fScrollPos, (m_vecRadios.size() - 4) * 200.f + 20.f);

	for (auto Button : m_vecRadios)
		dynamic_cast<RoomRadio*>(Button)->SetScrollPos(m_fScrollPos, (m_fScrollPos * 2.f) / WINCY);
}

int RoomRadioController::GetSelectRoomNumber()
{
	for (auto Button : m_vecRadios)
	{
		if (Button->GetButtonState() == BUTTON_STATE::ON)
			return Button->GetTag();
	}
	return -1;
}

void RoomRadioController::AddRoom(Scene* pScene, int iTag, bool bAddIdx)
{
	RoomRadio* pRadio = new RoomRadio(Core::GetInstance()->GetDevice(), Core::GetInstance()->GetCmdLst(), Renderer::GetInstance(),
		XMFLOAT4(178.f, 270.f + m_vecRadios.size() * 200.f, 1024.f, 180.f), "Room", "RoomSelect", "RoomSelect", pScene, iTag);
	pRadio->SetTag(iTag);

	pScene->PushObject(pRadio, OBJ_TYPE::OBJ_UI);
	m_vecRadios.push_back(pRadio);
	if (bAddIdx)
		m_lstRooms.push_back(iTag);
	SetRadioOthers();

}

void RoomRadioController::RemoveRoom(Scene* pScene, int iTag)
{
	auto iter = find(m_lstRooms.begin(), m_lstRooms.end(), iTag);
	if (iter == m_lstRooms.end())
		return;
	m_lstRooms.erase(iter);

	for (auto Radio : m_vecRadios)
	{
		pScene->RemoveObject(Radio, OBJ_TYPE::OBJ_UI);
		dynamic_cast<RoomRadio*>(Radio)->RemoveText();
		delete Radio;
	}
	m_vecRadios.clear();

	for (auto roomnum : m_lstRooms)
	{
		AddRoom(pScene, roomnum, false);
	}
	//auto eraseiter = m_vecRadios.end();
	//for (auto iter = m_vecRadios.begin(); iter != m_vecRadios.end(); ++iter)
	//{
	//	if ((*iter)->GetTag() == iTag)
	//	{
	//		eraseiter = iter;
	//		break;
	//	}
	//}
	//if (eraseiter == m_vecRadios.end())
	//	return;
	//pScene->RemoveObject((*eraseiter), OBJ_TYPE::OBJ_UI);
	//delete* eraseiter;
	//m_vecRadios.erase(eraseiter);

	
}
