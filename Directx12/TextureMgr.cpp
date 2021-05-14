#include "TextureMgr.h"
#include "DDSTexture.h"

TextureMgr* TextureMgr::m_pInstance = NULL;
void TextureMgr::BuildTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	DDSTexture* pTexture = NULL;

	pTexture = new DDSTexture(device, cmdLst, heap, "Stone01", L"../Resources/Stone01.dds");
	m_mapTextures["Stone01"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Tree02", L"../Resources/Tree02.dds");
	m_mapTextures["Tree02"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Lose", L"../Resources/Lose.dds");
	m_mapTextures["Lose"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Win", L"../Resources/Win.dds");
	m_mapTextures["Win"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "CrossHair", L"../Resources/CrossHair.dds");
	m_mapTextures["CrossHair"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise", L"../Resources/Noise.dds", false, 12);
	m_mapTextures["Noise"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise2", L"../Resources/Noise2.dds", false, 12);
	m_mapTextures["Noise2"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise3", L"../Resources/Noise3.dds", false, 12);
	m_mapTextures["Noise3"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise4", L"../Resources/Noise4.dds", false, 12);
	m_mapTextures["Noise4"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "BoomParticle", L"../Resources/BoomParticle.dds", false, 12);
	m_mapTextures["BoomParticle"] = pTexture;
	

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall", L"../Resources/FireBall.dds");
	m_mapTextures["FireBall"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall3", L"../Resources/FireBall3.dds");
	m_mapTextures["FireBall3"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall2", L"../Resources/FireBall2.dds");
	m_mapTextures["FireBall2"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall2", L"../Resources/Flames_FireEff.dds");
	m_mapTextures["Flames_FireEff"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "wizard_01", L"../Resources/Models/Characters/wizard_01/wizard_01.dds");
	m_mapTextures["wizard_01"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "SkyBox", L"../Resources/SkyBox/SkyBox_0.dds", true, 10);
	m_mapTextures["SkyBox"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Texture_01", L"../Resources/Models/Textures/Texture_01.dds");
	m_mapTextures["StaticMesh"] = pTexture;
	
	pTexture = new DDSTexture(device, cmdLst, heap, "StaticTexture", L"../Resources/Models/Textures/StaticTexture.dds");
	m_mapTextures["StaticTexture"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "StaticTexturePow", L"../Resources/Models/Textures/StaticTexturePow.dds");
	m_mapTextures["StaticTexturePow"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Lava", L"../Resources/Lava.dds");
	m_mapTextures["Lava"] = pTexture;
	pTexture = new DDSTexture(device, cmdLst, heap, "weapon", L"../Resources/Models/Textures/weapon.dds");
	m_mapTextures["weapon"] = pTexture;

	cout << "Load Texture Complete!" << endl;
}

void TextureMgr::BuildInitTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	DDSTexture* pTexture = NULL;

	pTexture = new DDSTexture(device, cmdLst, heap, "MainUi", L"../Resources/UI/Loading/MagicianWar.dds");
	m_mapTextures["MainUi"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "ButtonBase", L"../Resources/UI/Button/ButtonBase.dds");
	m_mapTextures["ButtonBase"] = pTexture;
	pTexture = new DDSTexture(device, cmdLst, heap, "ButtonMouseOn", L"../Resources/UI/Button/ButtonMouseOn.dds");
	m_mapTextures["ButtonMouseOn"] = pTexture;
	pTexture = new DDSTexture(device, cmdLst, heap, "ButtonOn", L"../Resources/UI/Button/ButtonOn.dds");
	m_mapTextures["ButtonOn"] = pTexture;
}

DDSTexture* TextureMgr::GetTexture(string textureName)
{
	auto iter = m_mapTextures.find(textureName);
	if (iter == m_mapTextures.end())
	{
		cout << "Error : No exist" << endl;
		return NULL;
	}

	return (*iter).second;
	//(*iter).second->PreRender(m_pCmdLst, m_ptrDescriptorHeap.Get());
}

HRESULT TextureMgr::PushTexture(string textureName, DDSTexture* texture)
{
	auto iter = m_mapTextures.find(textureName);
	if (iter != m_mapTextures.end())
	{
		cout << "Error : Duplicate texture name." << endl;
		return E_FAIL;
	}

	m_mapTextures[textureName] = texture;

	return S_OK;
}
