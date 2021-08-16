#include "SoundMgr.h"

SoundMgr* SoundMgr::m_pInstance = nullptr;

void SoundMgr::Initalize()
{
	m_pSystem = nullptr;
	
	System_Create(&m_pSystem);
	m_pSystem->init(32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFiles();
}

void SoundMgr::Release()
{
	for (auto& Pair : m_mapSound)
	{
		//delete[] Pair.first;
		Pair.second->release();
	}
	m_mapSound.clear();
	m_pSystem->release();
	m_pSystem->close();
}



void SoundMgr::PlaySound(string pSoundKey, SOUND_ID eID)
{
	map<string, FMOD::Sound*>::iterator iter = m_mapSound.find(pSoundKey);
	if (iter == m_mapSound.end())
		return;

	bool isPlay;

	if (m_pChannel[eID]->isPlaying(&isPlay))
	{
		m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannel[BGM]);

	}
	m_pSystem->update();
}

void SoundMgr::PlayBGM(string pSoundKey)
{
	map<string, FMOD::Sound*>::iterator iter = m_mapSound.find(pSoundKey);
	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannel[BGM]);
	m_pChannel[BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pSystem->update();
}

void SoundMgr::StopSound(SOUND_ID eID)
{
	m_pChannel[eID]->stop();
}

void SoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannel[i]->stop();
}

void SoundMgr::LoadSoundFile(string FileName, string FilePath)
{
	
	FMOD::Sound* pSound = nullptr;
	FMOD_RESULT eRes = m_pSystem->createSound(FilePath.c_str(), FMOD_DEFAULT, 0, &pSound);

	if (eRes == FMOD_OK)
	{
		m_mapSound.emplace(FileName, pSound);
	}
	m_pSystem->update();
}

void SoundMgr::LoadSoundFiles()
{
	LoadSoundFile("nextlevel", "../Resources/Sound/nextlevel.mp3");
	LoadSoundFile("blackmamba", "../Resources/Sound/blackmamba.mp3");
}
