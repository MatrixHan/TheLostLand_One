/**
*	created:		2013-5-10   6:15
*	filename: 		FKCThreadRoute
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <windows.h>
#include "../../Include/Network/FKCThreadRoute.h"
#include "../../include/FKITrace.h"
#include "../../Include/ResMgr/ResMgrCommon.h"
//------------------------------------------------------------------------
#if defined(_DEBUG) && !defined(__USE_MALLOC)
	#define __USE_MALLOC
	namespace std{class __default_alloc_template{};}
#endif
//------------------------------------------------------------------------
#include <algorithm>
#include <WinBase.h>
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
//------------------------------------------------------------------------
DWORD WINAPI ThreadProc( LPSTR lpData);
//------------------------------------------------------------------------
CThreadRoute::CThreadRoute()
{
	m_hCtrlEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_EventBuf[0] = m_hCtrlEvent;
	m_nUserEventCount = 0;
	
	m_dwEventIDLoop = 0;

	m_bSuspend = TRUE;
	m_bIsRunning = FALSE;
	m_dwThreadID = 0;
	m_hThread = NULL;

	Close();
}
//------------------------------------------------------------------------
CThreadRoute::~CThreadRoute()
{
	Close();

	if(m_hCtrlEvent)
	{
		CloseHandle(m_hCtrlEvent);
		m_hCtrlEvent = NULL;
	}
}
//------------------------------------------------------------------------
BOOL CreateThreadRoute(ITrace* pTrace,IThreadRoute** ppThreadRoute)
{
	if(ppThreadRoute == NULL)
		return FALSE;
	CThreadRoute *p = new CThreadRoute();
	if(p == NULL)
		return FALSE;
	if(!p->Create(pTrace))
	{
		delete p;
		return FALSE;
	}
	*ppThreadRoute = static_cast<IThreadRoute*>(p);
	return *ppThreadRoute != NULL;
}
//------------------------------------------------------------------------
BOOL CThreadRouteHelper::MyCreateThreadRoute(DWORD dwQueryVersion,ITrace* pTrace,IThreadRoute** ppThreadRoute)
{
	if(dwQueryVersion > VERSION_ThreadRoute)
	{
		TraceLn("Error : dllCreateThreadRoute() : dwQueryVersion = %d,but only %d supported",dwQueryVersion,VERSION_ThreadRoute);
		return FALSE;
	}
	return CreateThreadRoute(pTrace,ppThreadRoute);
}
//------------------------------------------------------------------------
BOOL CThreadRoute::Start()
{
	if(IsRunning())
		return TRUE;

	DWORD dwThreadID;
	if(	NULL == (m_hThread = CreateThread(( LPSECURITY_ATTRIBUTES) NULL,0,
			(LPTHREAD_START_ROUTINE)ThreadProc,
			(LPVOID)this,0,&dwThreadID)))
    {
		return FALSE;
    }
	SetRunning(TRUE);
	m_dwThreadID = dwThreadID;
	m_bSuspend = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::Stop(DWORD dwWaitTime)
{
	if(!IsRunning())
		return TRUE;
	ResumeThread();
	TriggerCtrlEvent(ctrlExit,dwWaitTime);
	if(IsRunning())
	{
		TraceLn("Warn : ThreadRoute::Stop : It can not be automaticly exit.Kill the thread\n");
		TerminateThread(m_hThread,-3);
		SetRunning(FALSE);
	}
	
	m_dwCtrlCmdMask = 0;
	m_dwThreadID = 0;
	CloseHandle(m_hThread);
	m_hThread = NULL;
	m_bSuspend = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
void CThreadRoute::Release()
{
	Stop(3000);
	delete this;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::IsRunning()
{
	return m_bIsRunning;
}
//------------------------------------------------------------------------
// ͬ�ϣ���֮ͬ��������IThreadRoute����һ��Event������IThreadRouteɾ��
BOOL CThreadRoute::AddEvent(IEventSink* pEventSink,/*out*/DWORD& dwEventID)
{
	Assert(pEventSink != NULL);

	m_LockEventBuf.Lock();
	BOOL bOK = m_nUserEventCount < MAX_USER_EVENT_COUNT;
	if(bOK)
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_EventBuf[m_nUserEventCount + SYS_EVENT_COUNT] = hEvent;

		++m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].dwEventID = m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].pEventSink = pEventSink;
		m_EventInfo[m_nUserEventCount].hLocalCreatedHandle = hEvent;
		dwEventID = m_dwEventIDLoop;

		++m_nUserEventCount;
	}
	m_LockEventBuf.Unlock();
	if(bOK)
	{
		if(!(bOK = TriggerCtrlEvent(ctrlFlipEventBuf,3000)))
			RemoveEvent(dwEventID);
	}
	return bOK;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::AddEvent(HANDLE hEvent,IEventSink* pEventSink,/*out*/DWORD& dwEventID)
{
	Assert(hEvent != NULL);
	Assert(pEventSink != NULL);

	m_LockEventBuf.Lock();
	BOOL bOK = m_nUserEventCount < MAX_USER_EVENT_COUNT;
	if(bOK)
	{
		
		m_EventBuf[m_nUserEventCount + SYS_EVENT_COUNT] = hEvent;

		++m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].dwEventID = m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].pEventSink = pEventSink;
		m_EventInfo[m_nUserEventCount].hLocalCreatedHandle = NULL;
		dwEventID = m_dwEventIDLoop;

		++m_nUserEventCount;
	}
	m_LockEventBuf.Unlock();

	if(bOK)
	{
		if(!(bOK = TriggerCtrlEvent(ctrlFlipEventBuf,3000)))
			RemoveEvent(dwEventID);
	}
	return bOK;
}
//------------------------------------------------------------------------
HANDLE CThreadRoute::GetEventHandle(DWORD dwEventID)
{
	HANDLE hEvent = NULL;
	m_LockEventBuf.Lock();
	for(int i = 0; i < m_nUserEventCount; i++)
	{
		if(m_EventInfo[i].dwEventID == dwEventID)
		{
			hEvent = m_EventBuf[i + SYS_EVENT_COUNT];
			break;
		}
	}
	m_LockEventBuf.Unlock();
#ifdef _DEBUG
	if(hEvent == NULL)
		OutputDebugString(L"Warn : CThreadRoute::GetEventHandle() : can not find event\n");
#endif
	return hEvent;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::RemoveEvent(DWORD dwEventID)
{
	m_LockEventBuf.Lock();
	BOOL bOK = FALSE;
	for(int i  = 0; i < m_nUserEventCount; i++)
	{
		if(m_EventInfo[i].dwEventID == dwEventID)
		{
			m_EventInfo[i].pEventSink->OnEventExit(dwEventID);
			if(m_EventInfo[i].hLocalCreatedHandle)
				CloseHandle(m_EventInfo[i].hLocalCreatedHandle);
			if(i != m_nUserEventCount - 1)
			{
				memmove(&m_EventInfo[i],&m_EventInfo[i+1],(m_nUserEventCount - i - 1) * sizeof(EVENT_INFO));
				memmove(&m_EventBuf[i + SYS_EVENT_COUNT],&m_EventBuf[i + SYS_EVENT_COUNT + 1],(m_nUserEventCount - i - 1) * sizeof(HANDLE));
			}
			--m_nUserEventCount;
			bOK = TRUE;
			break;
		}
	}
	m_LockEventBuf.Unlock();
	Assert(bOK);

	bOK = TriggerCtrlEvent(ctrlFlipEventBuf,3000);	// ��ʱδ�ܳɹ�
	Assert(bOK);
	return bOK;
}
//------------------------------------------------------------------------
// ���һЩ��Դ
void CThreadRoute::Close()
{
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	for(int i = 0; i < m_nUserEventCount; i++)
		m_EventInfo[i].Close();
	m_nUserEventCount = 0;
	memset(m_EventInfo,0,sizeof(m_EventInfo));

	m_dwCtrlCmdMask = 0;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::Create(ITrace *pTrace)
{
	Close();
	return TRUE;
}
//------------------------------------------------------------------------
void CThreadRoute::SetRunning(BOOL bRunning)
{
	m_LockEventBuf.Lock();
	m_bIsRunning = bRunning;
	m_LockEventBuf.Unlock();
}
//------------------------------------------------------------------------
// �߳��˳�
void CThreadRoute::OnTheadExit()
{
}
//------------------------------------------------------------------------
// ��pRoute��Event���ø��µ��̹߳���Event buffer
void UpdateEventBuf(CThreadRoute *pRoute,HANDLE* pEventBuf,EVENT_INFO* pEventInfo,int& nEventCount)
{
	nEventCount = pRoute->m_nUserEventCount + SYS_EVENT_COUNT;
	memcpy(pEventBuf,pRoute->m_EventBuf,nEventCount * sizeof(HANDLE));
	memcpy(pEventInfo,&pRoute->m_EventInfo[0],pRoute->m_nUserEventCount * sizeof(EVENT_INFO));
}
//------------------------------------------------------------------------
DWORD WINAPI ThreadProc( LPSTR lpData)
{
	CThreadRoute *pRoute = (CThreadRoute*)lpData;
	int			nEventCount;
	HANDLE		EventBuf[MAXIMUM_WAIT_OBJECTS];
	EVENT_INFO	EventInfo[MAX_USER_EVENT_COUNT];		// ǰSYS_EVENT_COUNT������ϵͳ�¼�
	
	UpdateEventBuf(pRoute,EventBuf,EventInfo,nEventCount);

	DWORD	dwThreadReturn = 0;
#ifdef _DEBUG
	wchar_t	szReturnMsg[128] = L"";
#endif

	DWORD dwTimeOutTick = -1,dwTimerNextTick = -1;
	pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);	// ��ʼ���붨ʱ����صı���
	while(TRUE)
	{
		DWORD dwEvent = WaitForMultipleObjectsEx(nEventCount,EventBuf,FALSE,dwTimeOutTick,TRUE);
		if(dwEvent >= (WAIT_OBJECT_0 + SYS_EVENT_COUNT) && dwEvent < (WAIT_OBJECT_0 + nEventCount) )
		{
			EVENT_INFO *pInfo = &EventInfo[dwEvent - WAIT_OBJECT_0 - SYS_EVENT_COUNT];
			pInfo->pEventSink->OnEvent(pInfo->dwEventID);
		}
		else if(dwEvent == WAIT_TIMEOUT)
		{
			pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);
			continue;
		}
		else if(dwEvent == WAIT_OBJECT_0)
		{ 
			
			if(pRoute->m_dwCtrlCmdMask & ctrlExit)
			{// �˳�
				pRoute->ClearCtrEventMask(ctrlExit);
#ifdef _DEBUG
				wcscpy(szReturnMsg,L"Set Exit Event");
#endif
				break;
			}
			if(pRoute->m_dwCtrlCmdMask & ctrlFlipEventBuf)
			{
				UpdateEventBuf(pRoute,EventBuf,EventInfo,nEventCount);
				pRoute->ClearCtrEventMask(ctrlFlipEventBuf);
			}
			if(pRoute->m_dwCtrlCmdMask & ctrlNewTimer)
			{// �������µ�timerʱ���������µ�timer��Ҫ����ִ��
				pRoute->ClearCtrEventMask(ctrlNewTimer);
				pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);
			}
			
		}
		else if(dwEvent == -1)
		{ // �������eventɾ����? ����ʵ��
			dwThreadReturn = 1;
#ifdef _DEBUG
			wcscpy(szReturnMsg,L" {Error => Find invalid event} ");
#endif
			break;
		}
		else 
		{
			dwThreadReturn = 2;
#ifdef _DEBUG
			wsprintf(szReturnMsg,L" {unknown dwEvent = %d} ",dwEvent);
#endif
			break;
		}

		// ��ʱ��鶨ʱ���Ƿ񵽴�
		if(dwTimerNextTick != -1)   //???? ���ܻᵼ���ܲ����timer���з�
		{
			dwTimeOutTick = dwTimerNextTick - GetTickCount();
			if(dwTimeOutTick == 0 || dwTimeOutTick > 0x7FFFFFFF)
			{// ʱ�䵽��
				pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);
			}
		}
	}
	pRoute->SetRunning(FALSE);
	pRoute->OnTheadExit();

#ifdef _DEBUG
	wchar_t szMsg[512];
	wsprintf(szMsg,L"\n=>>>>>>> Exit IThreadRoute,return = %d, %s<<<<<<<<=\n",dwThreadReturn,szReturnMsg);
	OutputDebugString(szMsg);
#endif
	return dwThreadReturn;
}
//------------------------------------------------------------------------
// ��ɾ�¼�ʱ������ThreadProc��CThreadRoute��Event buf���Ƶ����ѵ���ʱbuf��
BOOL CThreadRoute::TriggerCtrlEvent(ThreadCtrlCmd cmd,DWORD dwWaitTime)
{
	if(!IsRunning())
		return TRUE;
	m_LockEventBuf.Lock();
	m_dwCtrlCmdMask |= cmd;
	m_LockEventBuf.Unlock();
	SetEvent(m_hCtrlEvent);
	Sleep(0);
	DWORD dwLast = GetTickCount();
	while((m_dwCtrlCmdMask & cmd) && GetTickCount() - dwLast < dwWaitTime)
	{
		Sleep(1);	// ����ǰ�߳�ʱ��Ƭ���꣬ʹThreadProc�߳��л�������¼�����
					// ����������ɾ����¼�ʱ���¼����ö�Σ�������ֻ����һ�Σ��Ӷ�����ThreadProc������󼸴�EventBuf�ĸ���
					// �����ڴ�ѭ���ȴ�
	}
	if(m_dwCtrlCmdMask & cmd)
	{
		OutputDebugString(L"Warn : CThreadRoute::TriggerCtrlEvent() : time out!\n");
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::SetTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent,DWORD dwElapse,DWORD dwParam)
{
	if(pTimerSink == NULL)
		return FALSE;
	m_LockTimerList.Lock();
	CTimerNode t(pTimerSink,dwIDEvent,dwElapse,dwParam);
	list<CTimerNode>::iterator it = upper_bound(m_TimerList.begin(),m_TimerList.end(),t);
	m_TimerList.insert(it,t);
	m_LockTimerList.Unlock();

	if(IsRunning())
	{
		m_LockEventBuf.Lock();
		m_dwCtrlCmdMask |= ctrlNewTimer;
		m_LockEventBuf.Unlock();
		SetEvent(m_hCtrlEvent);	// ����Ҫ�ȴ��߳�ִ�����
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::KillTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent)
{
	m_LockEventBuf.Lock();
	m_TimerList.remove(CTimerNode(pTimerSink,dwIDEvent,0,0));
	m_LockEventBuf.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
// ����timer�¼�
void CThreadRoute::OnTriggerTimer(DWORD &dwNextTimeOut,DWORD& dwTimerNextTick)
{
	m_LockTimerList.Lock();
	if(m_TimerList.empty())
	{
		m_LockTimerList.Unlock();
		return;
	}
	list<CTimerNode>::iterator it = m_TimerList.begin();
	list<CTimerNode>::iterator end =m_TimerList.end();
	for(;it != end;)
	{
		dwNextTimeOut = (*it).dwNextTick - GetTickCount();
		if(dwNextTimeOut > 0 && dwNextTimeOut < 0x7FFFFFFF)	// ����timer��û��
			break;
		DWORD dwElapseTick = (*it).pTimerSink->OnTimer((*it).dwIDEvent,(*it).dwParam);
		if(dwElapseTick == -1)
		{
			m_TimerList.erase(it++);
			if(m_TimerList.empty())
			{
				dwTimerNextTick = -1;
				dwNextTimeOut = -1;
				m_LockTimerList.Unlock();
				return;
			}
		}
		else 
		{ // �Ѵ�timer����Ƶ���һ��ʱ���
			(*it).dwElapse = dwElapseTick;
			(*it).dwNextTick = GetTickCount() + dwElapseTick;
			
			list<CTimerNode>::iterator it_next = it;
			it_next++;
			list<CTimerNode>::iterator pos = upper_bound(it_next,m_TimerList.end(),*it);
			m_TimerList.insert(pos,*it);
			m_TimerList.erase(it);
			it = it_next;
		}
	}
	dwNextTimeOut =	m_TimerList.front().dwNextTick - GetTickCount();
	if(dwNextTimeOut > 0x7FFFFFFF)
		dwNextTimeOut = 0;

	dwTimerNextTick = m_TimerList.front().dwNextTick;
	m_LockTimerList.Unlock();
}
//------------------------------------------------------------------------
void CThreadRoute::ClearCtrEventMask(ThreadCtrlCmd ctrlClearMask)
{
	m_LockEventBuf.Lock();
	m_dwCtrlCmdMask &= ~ctrlClearMask;
	m_LockEventBuf.Unlock();
}
//------------------------------------------------------------------------