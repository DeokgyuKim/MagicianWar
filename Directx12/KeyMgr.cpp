#include "framework.h"
#include "KeyMgr.h"

// 일단 모든 키에 대해 적용
KeyMgr* KeyMgr::m_pInstance = nullptr;

bool KeyMgr::KeyPressing(int iKey)
{
    if (GetAsyncKeyState(iKey) & 0x8000)
        return true;

    return false;
}

bool KeyMgr::KeyDown(int iKey)
{
	if (!m_bKeyState[iKey] && (GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyState[iKey] = true;
		return true;
	}
	return false;
}

bool KeyMgr::KeyUp(int iKey)
{
	if (m_bKeyState[iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyState[iKey] = !m_bKeyState[iKey];
		return true;
	}
	return false;
}

void KeyMgr::KeyUpdate()
{
	for (int i = 0; i < VK_MAX; ++i)
	{
		if (m_bKeyState[i] && !(GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];
		if (!m_bKeyState[i] && (GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];
	}
}
