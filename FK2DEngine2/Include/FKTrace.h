/**
*	created:		2013-4-27   20:28
*	filename: 		FKTrace
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#ifndef _RICHEDIT_VER
#define _RICHEDIT_VER 0x0300
#endif
#ifndef UNICODE
#define UNICODE
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <richedit.h>
#include <queue>
#include <string>
#include "FKITrace.h"
#include "FKTraceMutex.h"
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
// ȫ�ֺ�������
LRESULT CALLBACK TraceWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//------------------------------------------------------------------------
// ��Ϣ�ṹ
typedef struct _TRACE_MSG
{
public:
	wstring		str;		// Trace�ı�
	COLORREF	crColor;	// Trace��ɫ
	int			nHeight;	// Trace�ֺ�
	DWORD		dwStyle;	// Trace����
	BOOL		bLn;		// �س�����
	BOOL		bShowTime;	// ��ʾʱ��

	_TRACE_MSG()
	{
		str.clear();
		crColor=TC_DEFAULT;
		nHeight=TH_DEFAULT;
		dwStyle=TS_DEFAULT;
		bLn=bShowTime=0;
	}

	~_TRACE_MSG()
	{
		str.erase(str.begin(),str.end());
		crColor=TC_DEFAULT;
		nHeight=TH_DEFAULT;
		dwStyle=TS_DEFAULT;
		bLn=bShowTime=0;
	}
} TRACE_MSG;
//------------------------------------------------------------------------
class CTrace : public ITrace
{
public:
	CTrace();
	~CTrace();
public:
	// ֱ�ӵ�����Ϣ��ʾ
	virtual BOOL FKShowTrace(const wchar_t* szMsg, COLORREF color = TC_DEFAULT, 
		int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
		BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// �̰߳�ȫ������Ϣ��ʾ
	virtual BOOL FKThreadSafeShowTrace(const wchar_t* szMsg, COLORREF color = TC_DEFAULT, 
		int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
		BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// �̰߳�ȫ�����Ϣ�����е���Ϣ
	virtual BOOL ThreadSafeGetTraceMsg();
	// �õ��Դ����ڵľ��
	virtual HWND GetTraceWindow(){return m_hWndTrace;}
	// �ͷ�(�����Դ����ڵ�����)
	virtual BOOL Release();
	// �̰߳�ȫ��ȡ��Ϣ�����е���Ϣ����������ʾ
	virtual BOOL ThreadSafeGetTraceMsgContext(wchar_t* szBuffer, int nSize, BOOL bRemoveMsg=FALSE);

	// ��̬�������������Ϣ���͵Ľӿڣ������ڳ������й����д򿪻�ر��ļ�Trace
	virtual BOOL SetOutputFile(bool bOutFile, const wchar_t* szFileName = 0);
	// ���õ���ѡ��
	BOOL SetOption(DWORD dwOutputStyle);
	// ��ʼ��
	BOOL Init(HWND hWndMain, HWND hRichEdit, const wchar_t* szFileName, HINSTANCE hInst, BOOL bExitAll = FALSE);
	// �����ڲ��Դ�����
	BOOL CreateTraceWindow(HINSTANCE hInst);
	// �����洢������Ϣ���ļ�
	BOOL CreateTraceFile(const wchar_t* szTraceFile);
	// ��RichEdit�������Ϣ
	BOOL AddToRichEdit(HWND hWnd, TRACE_MSG tm);
	// д�ļ�
	BOOL WriteString(const wchar_t* szMsg, int nSize);
	// �ߴ�ı�
	BOOL OnSize();
	// �����˳�
	BOOL IsExitAll();
	// ��Ϣ���
	TRACE_MSG SetMsg(const wchar_t* str, COLORREF color = TC_DEFAULT, int nHeight = TH_DEFAULT, 
		DWORD dwStyle = TS_DEFAULT, BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// �õ���Ӧ�ó���Ĵ��ھ��
	HWND GetMainWindow(){return m_hWndMain;}
	// ��ȡ����ʱ���ַ���
	BOOL GetDateTimeString(wchar_t* str);
	// ��ʾ��VC�ĵ��Դ���
	BOOL ShowToOutput(TRACE_MSG& tm);
	// ��ʾ���ļ�
	BOOL ShowToFile(TRACE_MSG& tm);
	// ��ʾ��RichEdit��
	BOOL ShowToRichEdit(HWND hWnd, TRACE_MSG& tm, BOOL bUseThread = FALSE);
public:
	DWORD			m_dwOutputStyle;		// ��������
	HWND			m_hWndMain;				// Ӧ�ó��������ھ��
	wchar_t*		m_pFileName;			// ���洢������Ϣ���ļ���(����·��)
	FILE*			m_hFile;				// ���洢������Ϣ���ļ����
	HWND			m_hRichEdit;			// ����ʾ������Ϣ�õ�Ӧ�ó���RichEdit�ؼ����
	HWND			m_hMyRichEdit;			// �ڲ������Ĵ����е�RichEdit�ؼ����
	HMODULE			m_hmodRichEdit;			// ����Riched32.dll���ģ����
	HWND			m_hWndTrace;			// �ڲ������Ĵ��ھ��
	CTraceMutex		m_mtxPrint, m_mtxQueue;	// ������,�ֱ�����ֱ�ӵ�����ʾ���̰߳�ȫ������ʾ
	queue<TRACE_MSG> m_MessageQueue;		// ��Ϣ����
	RECT			m_rcClient;				// ��Ⱦ��
	BOOL			m_bExitAll;				// �Ƿ������˳����������������˳���Ϣ
};
//------------------------------------------------------------------------