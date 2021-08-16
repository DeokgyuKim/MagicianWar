#include "TextController.h"
#include "Scene.h"

TextController::TextController(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* pRenderer, XMFLOAT4 xmfInfo, const char* pText, Scene* pScene)
{
	m_pDevice = device;
	m_pCmdLst = cmdLst;
	m_pRenderer = pRenderer;
	m_pScene = pScene;
	Initialize(xmfInfo, pText, pScene);
}

TextController::~TextController()
{
	Release();
}

void TextController::Initialize(XMFLOAT4 xmfInfo, const char* pText, Scene* pScene)
{
	m_iTextLength = strlen(pText);
	float fPosX = 0.f;
	for (int i = 0; i < m_iTextLength; ++i)
	{
		int idx, idxX, idxY;
		char c = pText[i];
		if (c == ' ')
		{
			fPosX += xmfInfo.w;
			continue;
		}
		else if (c >= 48 && c <= 57)
		{
			idx = c - 48;
		}
		else if (c >= 65 && c <= 90)
		{
			idx = c - 65 + 10;
		}
		else
			return;
		idxX = idx % 6;
		idxY = idx / 6;

		//Text»ý¼º
		XMFLOAT4 info;
		info.x = xmfInfo.x + fPosX ;
		info.y = xmfInfo.y;
		info.z = xmfInfo.z;
		info.w = xmfInfo.w;

		XMFLOAT2 xmfIdx;
		xmfIdx.x = idxX;
		xmfIdx.y = idxY;

		Text* pText = new Text(m_pDevice, m_pCmdLst, m_pRenderer, info, xmfIdx);
		m_vecTxt.push_back(pText);
		pScene->PushObject(pText, OBJ_TYPE::OBJ_UI);

		fPosX += xmfInfo.w * (1.f / 2.f);
	}
}

void TextController::UpdateChange()
{
}

void TextController::SetScrollPos(float fRatio)
{
	for (auto Txt : m_vecTxt)
		Txt->SetScrollPos(fRatio);
}

void TextController::SetTextColor(XMFLOAT3 fColor)
{
	for (auto Txt : m_vecTxt)
		Txt->SetTextColor(fColor);
}

void TextController::RemoveTexts(Scene* pScene)
{
	for (auto Txt : m_vecTxt)
	{
		pScene->RemoveObject(Txt, OBJ_UI);
		delete Txt;
	}
	m_vecTxt.clear();
}

void TextController::Release()
{
	//for (auto Txt : m_vecTxt)
	//{
	//	if (m_pScene->RemoveObject(Txt, OBJ_UI))
	//		delete Txt;
	//}
	//m_vecTxt.clear();
}

