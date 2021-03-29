#pragma once
#define VK_MAX 0xff

class KeyMgr
{
private:
	KeyMgr() { ZeroMemory(&m_bKeyState, sizeof(m_bKeyState)); }
	KeyMgr(const KeyMgr& rhs) = delete;
	~KeyMgr() {}
public:
	static KeyMgr* GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new KeyMgr();
		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static KeyMgr* m_pInstance;
public:
	bool KeyPressing(int iKey);
	bool KeyDown(int iKey);
	bool KeyUp(int iKey);
	void KeyUpdate();

private:
	bool m_bKeyState[VK_MAX];
};

