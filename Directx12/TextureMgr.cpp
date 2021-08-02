#include "TextureMgr.h"
#include "DDSTexture.h"
#include "Loading.h"

TextureMgr* TextureMgr::m_pInstance = NULL;
void TextureMgr::BuildTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, Loading* pLoading)
{
	DDSTexture* pTexture = NULL;

	pTexture = new DDSTexture(device, cmdLst, heap, "Stone01", L"../Resources/Stone01.dds");
	m_mapTextures["Stone01"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Tree02", L"../Resources/Tree02.dds");
	m_mapTextures["Tree02"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Lose", L"../Resources/Lose.dds");
	m_mapTextures["Lose"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Win", L"../Resources/Win.dds");
	m_mapTextures["Win"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "CrossHair", L"../Resources/CrossHair.dds");
	m_mapTextures["CrossHair"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Darkness", L"../Resources/UI/Character/Darkness.dds");
	m_mapTextures["Ui_Char_Darkness"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Ice", L"../Resources/UI/Character/Ice.dds");
	m_mapTextures["Ui_Char_Cold"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Fire", L"../Resources/UI/Character/Fire.dds");
	m_mapTextures["Ui_Char_Fire"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Base", L"../Resources/UI/Button/Button/Base.dds");
	m_mapTextures["Ui_Char_Base"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Click", L"../Resources/UI/Button/Button/Click.dds");
	m_mapTextures["Ui_Char_Click"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Mouse", L"../Resources/UI/Button/Button/Mouse.dds");
	m_mapTextures["Ui_Char_Mouse"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "BlueTeam", L"../Resources/UI/Text/BlueTeam.dds");
	m_mapTextures["Ui_Text_BlueTeam"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "JoinRoom", L"../Resources/UI/Text/JoinRoom.dds");
	m_mapTextures["Ui_Text_JoinRoom"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "MakeRoom", L"../Resources/UI/Text/MakeRoom.dds");
	m_mapTextures["Ui_Text_MakeRoom"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Ready", L"../Resources/UI/Text/Ready.dds");
	m_mapTextures["Ui_Text_Ready"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "RedTeam", L"../Resources/UI/Text/RedTeam.dds");
	m_mapTextures["Ui_Text_RedTeam"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Start", L"../Resources/UI/Text/Start.dds");
	m_mapTextures["Ui_Text_Start"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Lobby", L"../Resources/UI/Lobby/Lobby.dds");
	m_mapTextures["Lobby"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Lobby", L"../Resources/UI/Lobby/Lobby.dds");
	m_mapTextures["Lobby"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Room", L"../Resources/UI/Lobby/Room.dds");
	m_mapTextures["Room"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "RoomSelect", L"../Resources/UI/Lobby/RoomSelect.dds");
	m_mapTextures["RoomSelect"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "RoomBase", L"../Resources/UI/Lobby/RoomBase.dds");
	m_mapTextures["RoomBase"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise", L"../Resources/Noise.dds", false, 12);
	m_mapTextures["Noise"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise2", L"../Resources/Noise2.dds", false, 12);
	m_mapTextures["Noise2"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise3", L"../Resources/Noise3.dds", false, 12);
	m_mapTextures["Noise3"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Noise4", L"../Resources/Noise4.dds", false, 12);
	m_mapTextures["Noise4"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "BoomParticle", L"../Resources/BoomParticle.dds", false, 12);
	m_mapTextures["BoomParticle"] = pTexture;
	pLoading->AddCount();
	

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall", L"../Resources/FireBall.dds");
	m_mapTextures["FireBall"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall3", L"../Resources/FireBall3.dds");
	m_mapTextures["FireBall3"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall2", L"../Resources/FireBall2.dds");
	m_mapTextures["FireBall2"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "FireBall2", L"../Resources/Flames_FireEff.dds");
	m_mapTextures["Flames_FireEff"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "wizard_Fire", L"../Resources/Models/Characters/wizard_Fire/wizard_Fire.dds");
	m_mapTextures["wizard_Fire"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "wizard_Fire", L"../Resources/Models/Characters/wizard_Darkness/wizard_Darkness.dds");
	m_mapTextures["wizard_Darkness"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "wizard_Fire", L"../Resources/Models/Characters/wizard_Black/wizard_Black.dds");
	m_mapTextures["wizard_Black"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "wizard_Fire", L"../Resources/Models/Characters/wizard_Cold/wizard_Cold.dds");
	m_mapTextures["wizard_Cold"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "SkyBox", L"../Resources/SkyBox/SkyBox_0.dds", true, 10);
	m_mapTextures["SkyBox"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Texture_01", L"../Resources/Models/Textures/Texture_01.dds");
	m_mapTextures["StaticMesh"] = pTexture;
	pLoading->AddCount();
	
	pTexture = new DDSTexture(device, cmdLst, heap, "StaticTexture", L"../Resources/Models/Textures/StaticTexture.dds");
	m_mapTextures["StaticTexture"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "StaticTexturePow", L"../Resources/Models/Textures/StaticTexturePow.dds");
	m_mapTextures["StaticTexturePow"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "Lava", L"../Resources/Lava.dds");
	m_mapTextures["Lava"] = pTexture;
	pLoading->AddCount();

	pTexture = new DDSTexture(device, cmdLst, heap, "weapon", L"../Resources/Models/Textures/weapon.dds");
	m_mapTextures["weapon"] = pTexture;
	pLoading->AddCount();

	cout << "Load Texture Complete!" << endl;
}

void TextureMgr::BuildInitTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap)
{
	DDSTexture* pTexture = NULL;

	pTexture = new DDSTexture(device, cmdLst, heap, "MainLogo", L"../Resources/UI/Loading/MainLogo.dds");
	m_mapTextures["MainLogo"] = pTexture;

	pTexture = new DDSTexture(device, cmdLst, heap, "LoadingBar", L"../Resources/UI/Loading/LoadingBar.dds");
	m_mapTextures["LoadingBar"] = pTexture;
	
	pTexture = new DDSTexture(device, cmdLst, heap, "ButtonBase", L"../Resources/UI/Button/ButtonBase.dds");
	m_mapTextures["ButtonBase"] = pTexture;
	pTexture = new DDSTexture(device, cmdLst, heap, "ButtonMouseOn", L"../Resources/UI/Button/ButtonMouseOn.dds");
	m_mapTextures["ButtonMouseOn"] = pTexture;
	pTexture = new DDSTexture(device, cmdLst, heap, "ButtonOn", L"../Resources/UI/Button/ButtonOn.dds");
	m_mapTextures["ButtonOn"] = pTexture;
	pTexture = new DDSTexture(device, cmdLst, heap, "No", L"../Resources/UI/Text/No.dds");
	m_mapTextures["Ui_Text_No"] = pTexture;
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
