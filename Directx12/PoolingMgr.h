#pragma once
#include "framework.h"

class Object;
class Renderer;
class PoolingMgr
{
private:
	PoolingMgr(){}
	~PoolingMgr() { Release(); }
public:
	static PoolingMgr* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new PoolingMgr();
		return m_pInstance;
	}
	static void			DestroyInstance() {
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static PoolingMgr* m_pInstance;
public:
	void		InitPoolingObject(ID3D12Device* device, ID3D12GraphicsCommandList* cmdLst, Renderer* renderer);
	Object*		CreatePoolObject(string instName);
	void		DeletePoolObject(Object* obj);
private:
	void		Release();
private:
	unordered_map<string, list<Object*>> m_mapPoolObject;
};

