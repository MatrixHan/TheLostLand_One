/**
*	created:		2013-4-16   5:04
*	filename: 		BaseThread
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "inc.h"
#include <stdio.h>
//------------------------------------------------------------------------
enum	_threadlevel_
{
	TL_IDLE = THREAD_PRIORITY_IDLE,
	TL_REALTIME = (REALTIME_PRIORITY_CLASS<<16) | THREAD_PRIORITY_NORMAL,
	TL_TIME_CRITICAL	=	THREAD_PRIORITY_TIME_CRITICAL,
	TL_HIGHEST = (HIGH_PRIORITY_CLASS << 16)|THREAD_PRIORITY_HIGHEST,
	TL_HIGH = (HIGH_PRIORITY_CLASS << 16 ) | THREAD_PRIORITY_NORMAL,
	TL_NORMAL = (NORMAL_PRIORITY_CLASS << 16) | THREAD_PRIORITY_NORMAL,
	TL_LOW = (NORMAL_PRIORITY_CLASS << 16 ) | THREAD_PRIORITY_LOWEST,
	TL_LOWEST = (IDLE_PRIORITY_CLASS << 16) | THREAD_PRIORITY_NORMAL,
};
//------------------------------------------------------------------------
class CBaseThread
{
public:
	CBaseThread(void);
	virtual ~CBaseThread(void);
	static int m_nTreadCreated;
	static int m_nTreadRun;
private:
	// �̴߳�����
	static DWORD ThreadProc(LPVOID lpParam);
protected:
	// �������̴߳�����
	virtual DWORD Proc(LPVOID lpParam);
	LPVOID		m_lpParam;
	HANDLE		m_hThread;
	DWORD		m_dwThreadId;
	HANDLE		m_hExitEvent;
	DWORD		m_dwRunTimes;
	DWORD		m_dwRunTimesKilo;
	SYSTEMTIME	m_stStartTime;
public:
	VOID GetTimeInfo( DWORD & dwRunTimes, DWORD & dwRunTimesKilo, DWORD & dwRunSeconds );
	// �����߳�
	virtual HANDLE Create(DWORD dwStackSize, LPVOID lpParam, DWORD* pId, DWORD	dwThreadPriorityLevel = TL_NORMAL);
	// ��ȫ�˳��߳�
	BOOL SafeTerminate(DWORD dwWaitTime);
	// �����߳��Ƿ��Ծ
	virtual BOOL ThreadActive(void);
	// ���������߳�
	virtual BOOL Terminate(void);
protected:
	virtual VOID OnStart(void);
	virtual VOID OnExit(void);
public:
	virtual BOOL WaitForTerminate(void);
};
//------------------------------------------------------------------------