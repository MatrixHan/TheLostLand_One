/**
*	created:		2013-5-10   2:04
*	filename: 		FKThreadRoute
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
/*
	�̹߳���
		����һ���߳̿���ͬʱ��������63���ⲿ�¼�������Խ�ͨѶ�¼��ŵ����߳��У�
		��ͨѶ�¼�����ʱ�����¼�֪ͨת��ͨѶ�㴦��
			��һ����Ҫ��;�Ƕ�ʱ�����񣬲���Ҫ�����ںˣ������ڽ���ʱ�����п���ʵ�ֲ������������Ķ�ʱ��
		��ʱ����������ṩ���õ�״̬����
		
	ʵ�ַ�����
			��ÿ���¼��ŵ�WaitForMultiObject��EventBuf�У���ĳ�¼�����ʱ������ָ���ĵ���

	�ṹͼ��
			������� <=> IThreadRoute <=> ���ģ��

			���������һ��dialog����һ��service
	
	��;��
			1�������ͨѶClientSocket�ҽӵ�һ���߳��У�
			2��һ�����������������Ҷ��������
 */
//------------------------------------------------------------------------
#define INVALID_EVENT_ID		-1
#define VERSION_ThreadRoute		1
//------------------------------------------------------------------------
struct ITrace;
struct IEventSink;
struct ITimerEventSink;
//------------------------------------------------------------------------
struct IThreadRoute
{
	// �߳�״̬����
	//------------------------------------------------------------------------
	// �����߳�
	// �ɽ����е����ģ��ӵ�IThreadRoute�Ժ������̣߳�Ҳ�����������Ĺ��������ģ�齫���ѵ��¼���̬��ӽ���
	virtual BOOL Start() = NULL;
	
	// ������ֹͣ�߳�,����ָ���ȴ�ʱ��dwWaitTick���Ժ���Ϊ��λ
	// ע�⣺�˽Ӻ���ֻ�ܹ���������ã�ThreadRoute�̱߳�����ֹͣ���ѣ�
	//		 ���ģ�����ThreadRoute����ͬһ�̣߳����Ե��ô˺�������������
	virtual BOOL Stop(DWORD dwWaitTime = 3000) = NULL;
	
	// �߳�����û��?
	virtual BOOL IsRunning() = NULL;

	// ��ȡ�߳�Handler
	virtual HANDLE GetThreadHandle() = NULL;

	// �����߳�
	virtual DWORD ResumeThread() = NULL;

	// ��ͣ�߳�
	virtual DWORD SuspendThread () = NULL;

	// �߳��¼�����
	//------------------------------------------------------------------------
	/*
	 	���ܣ����ģ�齫���ѵ�һ���¼��ҽӵ���ǰ�߳���
		���룺
			hEvent		������hEvent��socket�����
			pEventSink	�������¼�����ʱ���ص��˽ӿ�
		���أ�
			dwEventID	�������¼�����ʱ������IEventSink::OnEvent(dwEventID)
	*/
	virtual BOOL AddEvent(HANDLE hEvent,IEventSink* pEventSink,/*out*/DWORD& dwEventID) = NULL;

	// ͬ�ϣ���֮ͬ������hEvent��IThreadRoute������ɾ�����Ƽ�ʹ������ӿ�
	virtual BOOL AddEvent(IEventSink* pEventSink,/*out*/DWORD& dwEventID) = NULL;

	// ����hEvent����IThreadRoute�Զ������¼�ʱ���û�������Ҫ���Ҵ˾��
	virtual HANDLE GetEventHandle(DWORD dwEventID) = NULL;
	
	// ���һ���¼�����
	virtual BOOL RemoveEvent(DWORD dwEventID) = NULL;
	

	// �̶߳�ʱ������
	//------------------------------------------------------------------------
	// ���Ӷ�ʱ��
	// ע�⣺�������ͬһ����ʱ��ʱ��ÿ����ʱ������Ч
	//       dwElapse�����ƣ�dwElapse > 0 && dwElapes < 0x7FFFFFFF
	virtual BOOL SetTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent,DWORD dwElapse,DWORD dwParam = 0) = NULL;
	
	// ���ģ������IThreadRoute�����ʱ����
	virtual BOOL KillTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent) = NULL;

	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
//�����ģ��
struct IEventSink
{
	// ���¼�������ʱ���ص��˽ӿڣ�����dwEventID����������һ���ӿڸ����Event��
	virtual void OnEvent(DWORD dwEventID) = NULL;
	
	// ���߳��˳�ʱ��hEvent��Чʱ�����ô˽ӿڣ�һ�㲻��ʵ�ִ˽ӿ�
	virtual void OnEventExit(DWORD dwEventID){;}
};
//------------------------------------------------------------------------
// ʱ�������ʵ�ֵĽӿ�
// Ϊ�����mud�е�ITimerSink���ͻ������Ϊ�ĳ�ITimerEventSink
struct ITimerEventSink
{
	// ʱ���¼�����
	// ����ֵΪ�´�ִ�д�timer��ʱ�䣬��msΪ��λ
	// ���ͣ�ô�timer���践��-1��ϵͳ�Զ�KillTimer()
	virtual DWORD OnTimer(DWORD dwTimerID = 0,DWORD dwParam = 0) = NULL;
	// ���߳��˳�ʱ�����ô˽ӿڣ���KillTimer()ʱ���ص��˽ӿڡ�һ�㲻��ʵ�ִ˽ӿ�
	virtual void OnExitTimer(DWORD dwTimerID){;}
};
//------------------------------------------------------------------------
// ����ClientSocket.lib�е�CreateThreadRoute()����һ��IThreadRoute����
extern "C"  BOOL CreateThreadRoute(ITrace* pTrace,IThreadRoute** ppThreadRoute);
//------------------------------------------------------------------------
class CThreadRouteHelper
{
public:
	CThreadRouteHelper()
	{
		m_pThreadRoute = NULL;
	}
	~CThreadRouteHelper()
	{
		Close();
	}
	void Close()
	{
		if(m_pThreadRoute)
		{
			m_pThreadRoute->Release();
			m_pThreadRoute = NULL;
		}
	}
	BOOL Create(ITrace* pTrace)
	{
		Close();

		if(!MyCreateThreadRoute(VERSION_ThreadRoute,pTrace,&m_pThreadRoute))
		{
			return FALSE;
		}
		return m_pThreadRoute != NULL;
	}
public:
	BOOL MyCreateThreadRoute(DWORD dwQueryVersion,ITrace* pTrace,IThreadRoute** ppThreadRoute);
public:
	IThreadRoute* m_pThreadRoute;
};
//------------------------------------------------------------------------
// ͨ��lib��ʽ����IThreadRoute����
class CThreadRouteObject
{
public:
	CThreadRouteObject()
	{
		m_pThreadRoute = NULL;
	}
	~CThreadRouteObject()
	{
		Close();
	}
	void Close()
	{
		if(m_pThreadRoute)
		{
			m_pThreadRoute->Release();
			m_pThreadRoute = NULL;
		}
	}
	BOOL Create(ITrace* pTrace)
	{
		Close();
		return CreateThreadRoute(pTrace,&m_pThreadRoute);
	}

	IThreadRoute*operator->()
	{
		return m_pThreadRoute;
	}
	IThreadRoute* m_pThreadRoute;
};
//------------------------------------------------------------------------
