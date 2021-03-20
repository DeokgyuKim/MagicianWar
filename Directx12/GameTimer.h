#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class GameTimer
{ // Timer�� �ϳ��� �����ؼ� �����̰� ���� �÷��� ���۽� ����� ������ ����ϱ� �̱���

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
	void Reset(); // �޽��� ���� ���� �� ȣ��
	void Start(); // �Ͻ����� ������ ȣ��
	void Stop();  // �Ͻ����� �� ȣ��
	void Tick(float fLockFPS = 0.0f);  // �� �����Ӹ��� ȣ��


	float TotalTime() const; // �ʴ���
	float GetDeltaTime() const; // �ʴ���


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

