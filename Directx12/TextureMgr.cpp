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

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise", L"../Resources/Noise.dds", false, 12);
	m_mapTextures["Noise"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise2", L"../Resources/Noise2.dds", false, 12);
	m_mapTextures["Noise2"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise3", L"../Resources/Noise3.dds", false, 12);
	m_mapTextures["Noise3"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall", L"../Resources/FireBall.dds");
	m_mapTextures["FireBall"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall2", L"../Resources/FireBall2.dds");
	m_mapTextures["FireBall2"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall3", L"../Resources/FireBall3.dds");
	m_mapTextures["FireBall3"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "wizard_01", L"../Resources/Models/Characters/wizard_01/wizard_01.dds");
	m_mapTextures["wizard_01"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "SkyBox", L"../Resources/SkyBox/SkyBox_0.dds", true, 10);
	m_mapTextures["SkyBox"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "Texture_01", L"../Resources/Models/Textures/Texture_01.dds");
	m_mapTextures["StaticMesh"] = pTexture;
	
	pTexture = new DDSTexture(device, cmdLst, heap, "StaticTexture", L"../Resources/Models/Textures/StaticTexture.dds");
	m_mapTextures["StaticTexture"] = pTexture;
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
