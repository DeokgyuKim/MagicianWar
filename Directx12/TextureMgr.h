#pragma once
#include "framework.h"

class Loading;
class DDSTexture;
class TextureMgr
{
private:
	TextureMgr() {}
	TextureMgr(const TextureMgr& rhs) {}
	~TextureMgr() { Release(); }
public:
	static TextureMgr* GetInstance()
	{
		if (m_pInstance == NULL)
			m_pInstance = new TextureMgr();
		return m_pInstance;
	}
	static void		DestroyInstance()
	{
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static TextureMgr* m_pInstance;
private:
	void Release();
public:
	void		BuildTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap, Loading* pLoading);
	void		BuildInitTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, ID3D12DescriptorHeap* heap);
	DDSTexture* GetTexture(string textureName);
	HRESULT		PushTexture(string textureName, DDSTexture* texture);

private:
	unordered_map<string, DDSTexture*>	m_mapTextures;
};

