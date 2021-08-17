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



int SoundMgr::PlaySound(string pSoundKey, SOUND_ID eID)
{
	map<string, FMOD::Sound*>::iterator iter = m_mapSound.find(pSoundKey);
	if (iter == m_mapSound.end())
		return -1;

	bool isPlay;

	if (m_pChannel[eID]->isPlaying(&isPlay))
	{
		m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannel[eID]);
	}
	m_pSystem->update();
	return eID;
}

int SoundMgr::PlaySound(string pSoundKey)
{
	map<string, FMOD::Sound*>::iterator iter = m_mapSound.find(pSoundKey);
	if (iter == m_mapSound.end())
		return -1;

	bool isPlay;

	if (m_pChannel[m_iEffCurChannel]->isPlaying(&isPlay))
	{
		m_pSystem->playSound(iter->second, NULL, FALSE, &m_pChannel[m_iEffCurChannel]);
	}
	m_pSystem->update();
	m_iEffCurChannel = m_iEffCurChannel + 1;
	if (m_iEffCurChannel >= SOUND_ID::MAXCHANNEL)
		m_iEffCurChannel = SOUND_ID::EFFSTART;
	return m_iEffCurChannel - 1;
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

bool SoundMgr::IsPlaying(SOUND_ID eID)
{
	bool play = false;
	m_pChannel[eID]->isPlaying(&play);
	return play;
}

void SoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannel[i]->stop();
}

void SoundMgr::SoundControlbyChannel(SOUND_ID eID, float fVolume)
{
	m_pChannel[eID]->setVolume(fVolume);
}

void SoundMgr::LoadSoundFile(string FileName, string FilePath)
{
	
	FMOD::Sound* pSound = nullptr;
	FMOD_RESULT eRes = m_pSystem->createSound(FilePath.c_str(), FMOD_DEFAULT, 0, &pSound);

	if (eRes == FMOD_OK)
	{
		m_mapSound.emplace(FileName, pSound);
		cout << "Load Sound SUCCESS FileName : " << FileName << endl;
	}
	else
		cout << "Load Sound FAILED FileName : " << FileName << endl;
	m_pSystem->update();
}

void SoundMgr::LoadSoundFiles()
{
	//bgm
	LoadSoundFile("nextlevel", "../Resources/Sound/nextlevel.mp3");
	LoadSoundFile("blackmamba", "../Resources/Sound/blackmamba.mp3");

	//Mouseclick
	LoadSoundFile("MouseClick", "../Resources/Sound/MouseClick.wav");

	//Att
	LoadSoundFile("Attack1", "../Resources/Sound/Attack1.wav");
	LoadSoundFile("Attack2", "../Resources/Sound/Attack2.wav");
	LoadSoundFile("Attack3", "../Resources/Sound/Attack3.wav");
	LoadSoundFile("Attack4", "../Resources/Sound/Attack4.wav");
	LoadSoundFile("Attack5", "../Resources/Sound/Attack5.wav");
	LoadSoundFile("MyHitMonster", "../Resources/Sound/MyHitMonster.wav");

	//Walk
	LoadSoundFile("Walk1", "../Resources/Sound/Walk1.wav");
	LoadSoundFile("Walk2", "../Resources/Sound/Walk2.wav");

	//Skill
	LoadSoundFile("FireWall", "../Resources/Sound/FireWall.mp3");
	LoadSoundFile("Meteor", "../Resources/Sound/Meteor.mp3");
	LoadSoundFile("Boom", "../Resources/Sound/Boom.wav");

	LoadSoundFile("IceField", "../Resources/Sound/IceField.mp3");
	LoadSoundFile("IceFreezeMake", "../Resources/Sound/IceFreezeMake.wav");
	LoadSoundFile("IceFreezeDestroy", "../Resources/Sound/IceFreezeDestroy.wav");
	LoadSoundFile("Crevas", "../Resources/Sound/Crevas.wav");

	LoadSoundFile("BlackHole", "../Resources/Sound/BlackHole.wav");
	LoadSoundFile("Hatred", "../Resources/Sound/Hatred.wav");
}
