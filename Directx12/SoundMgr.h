#pragma once
#include "framework.h"


class SoundMgr
{
private:
	SoundMgr() {}
	~SoundMgr() { Release(); }
public:

	static SoundMgr* GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new SoundMgr();
		return m_pInstance;
	}
	static void	DestroyInstance() {
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static SoundMgr* m_pInstance;

private:
	void Release();
public:
	void Initalize();
public:
	void PlaySound(TCHAR* pSoundKey, SOUND_ID eID);
	void PlayBGM(TCHAR* pSoundKey);
	void StopSound(SOUND_ID eID);
	void StopAll();
private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannel[MAXCHANNEL];
	// 사운드 채널 객체 및 장치를 관리하는 객체. 
	FMOD_SYSTEM* m_pSystem;
};

class CStrCmp
{
public:
	CStrCmp() {}
	CStrCmp(const TCHAR* pStr)
		:m_pString(pStr)
	{}
	~CStrCmp() {}

public:
	template<class T>
	bool operator()(T& rPair)
	{
		return !lstrcmp(m_pString, rPair.first);
	}
private:
	const TCHAR* m_pString;
};