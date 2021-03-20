#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class GameTimer
{ // Timer도 하나만 생성해서 쓸것이고 게임 플레이 시작시 만들고 끝나면 지우니까 싱글턴

private:
	GameTimer();
	GameTimer(const GameTimer& rhs) = delete;
	~GameTimer() {}
public:
	static GameTimer* GetInstance()
	{
		if (m_pInstance == NULL)
			m_pInstance = new GameTimer();
		return m_pInstance;
	}
	static void DestoryInstance()
	{
		if (m_pInstance != NULL)
			delete m_pInstance;
		m_pInstance = NULL;
	}
private:
	static GameTimer* m_pInstance;
public:
	void Reset(); // 메시지 루프 돌기 전 호출
	void Start(); // 일시중지 해제시 호출
	void Stop();  // 일시중지 시 호출
	void Tick(float fLockFPS = 0.0f);  // 매 프레임마다 호출


	float TotalTime() const; // 초단위
	float GetDeltaTime() const; // 초단위


private:
	double							m_fTimeScale;
	float							m_fTimeElapsed;

	__int64							m_nBasePerformanceCounter;
	__int64							m_nPausedPerformanceCounter;
	__int64							m_nStopPerformanceCounter;
	__int64							m_nCurrentPerformanceCounter;
	__int64							m_nLastPerformanceCounter;

	__int64							m_nPerformanceFrequencyPerSec;

	float							m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG							m_nSampleCount;

	unsigned long					m_nCurrentFrameRate;
	unsigned long					m_nFramesPerSecond;
	float							m_fFPSTimeElapsed;

	bool							m_bStopped;
};

