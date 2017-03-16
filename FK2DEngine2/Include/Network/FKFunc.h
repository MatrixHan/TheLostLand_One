/**
*	created:		2013-5-10   2:34
*	filename: 		FKFunc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CCriticalSectionLock
{
public:
	CCriticalSectionLock()
	{
		InitializeCriticalSection(&m_sect);
		m_bNoneLock = FALSE;
	}
public:
	CRITICAL_SECTION m_sect;
	BOOL m_bNoneLock;
public:
	inline void Lock()
	{
		if(m_bNoneLock)
			return;
		EnterCriticalSection(&m_sect);
	}
	inline void Unlock()
	{
		if(m_bNoneLock)
			return;
		LeaveCriticalSection(&m_sect);
	}
	// ��������Ч����Ϊͬһ�߳�ʱ������ʱ��������
	void SetNoneLock(BOOL bNoneLock)
	{
		m_bNoneLock = bNoneLock;
	}
public:
	virtual ~CCriticalSectionLock(){}
};
//------------------------------------------------------------------------
inline char *MyAllocMemory(int nLen)
{ // �Ż��ڴ����
	if(nLen <= 0)
		return NULL;
	nLen = (nLen / 64 + 1) * 64;
	return new char[nLen];
}
//------------------------------------------------------------------------