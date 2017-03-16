/**
*	created:		2013-4-16   4:54
*	filename: 		xThread
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "xinc.h"
#include <stdexcept>
//------------------------------------------------------------------------
class xThread
{
public:
	xThread(void);
	virtual ~xThread(void);

	///�����߳�
	BOOL Start(LPVOID lpParam = NULL, BOOL bSuspended = FALSE);

	///�����߳�
	BOOL Resume(void);

	/// �����߳�
	BOOL Suspend(void);

	///��ֹ�߳�
	void Terminate(void);

	///�ж��߳��Ƿ�����ֹ
	BOOL IsTerminated(void);

	///�߳��Ƿ�������
	BOOL IsStarted(void);

	///�ȴ��߳��˳�
	void WaitFor(void);

	HANDLE	getHandle(){ return m_hThread;}
	DWORD	getThreadID(){ return m_dwThreadID;}
	
protected:

	///�̵߳���������
	virtual void Execute(LPVOID lpParam) = 0;

	///Terminate�¼�
	virtual void OnTerminated(BOOL bExcepted = FALSE ) {};

	virtual void OnExecuteException(const exception& e){}

	///��־�Ƿ�����ֹ
	volatile BOOL m_bTerminated;

private:
	LPVOID	m_lpParam;

	///�̺߳�
	DWORD m_dwThreadID;

	///�߳̾��
	HANDLE m_hThread;

	///��־�Ƿ��ѹ���
	BOOL m_bIsSuspended;

	///�߳�����
	static unsigned WINAPI ThreadProc(LPVOID pParam);
};
//------------------------------------------------------------------------