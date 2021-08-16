#include "SoundMgr.h"
#include <io.h>
//#include "Fmod/"

SoundMgr* SoundMgr::m_pInstance = nullptr;

void SoundMgr::Initalize()
{
	m_pSystem = nullptr;

	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
}

void SoundMgr::Release()
{
	for (auto& Pair : m_mapSound)
	{
		delete[] Pair.first;
		FMOD_Sound_Release(Pair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}



void SoundMgr::PlaySound(TCHAR* pSoundKey, SOUND_ID eID)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStrCmp(pSoundKey));
	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL isPlay;

	if (FMOD_Channel_IsPlaying(m_pChannel[eID], &isPlay))
	{
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannel[eID]);

	}
	FMOD_System_Update(m_pSystem);
}

void SoundMgr::PlayBGM(TCHAR* pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_mapSound.begin(), m_mapSound.end(), CStrCmp(pSoundKey));
	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannel[BGM]);
	FMOD_Channel_SetMode(m_pChannel[BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void SoundMgr::StopSound(SOUND_ID eID)
{
	FMOD_Channel_Stop(m_pChannel[eID]);
}

void SoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannel[i]);
}

void SoundMgr::LoadSoundFile()
{
	_finddata_t fd;

	long Handle = _findfirst("../Sound/*.*", &fd);

	if (Handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Sound/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLen = strlen(fd.name) + 1;
			TCHAR* pSoundKey = new TCHAR[iLen];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLen);

			MultiByteToWideChar(CP_ACP, 0, fd.name, iLen, pSoundKey, iLen);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _findnext(Handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
}
