#pragma once
#include "framework.h"
#include <io.h>
#include "fmod.hpp"
#include "fmod_errors.h"

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
	void PlaySound(string pSoundKey, SOUND_ID eID);
	void PlayBGM(string pSoundKey);
	void StopSound(SOUND_ID eID);
	void StopAll();
public:
	void LoadSoundFile(string FileName, string FilePath);
public:
	void LoadSoundFiles();
private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<string, FMOD::Sound*> m_mapSound;
	// ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel* m_pChannel[MAXCHANNEL];
	// ���� ä�� ��ü �� ��ġ�� �����ϴ� ��ü. 
	FMOD::System* m_pSystem;
};