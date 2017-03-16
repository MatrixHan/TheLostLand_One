/**
*	created:		2013-4-27   20:31
*	filename: 		FKTrace
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <tchar.h>
#include "../include/FKTrace.h"
//------------------------------------------------------------------------
CTrace::CTrace()
{
	m_dwOutputStyle = 0L;
	m_hWndMain = NULL;
	m_pFileName	= NULL;
	m_hFile = NULL;
	m_hRichEdit = NULL;

	m_hMyRichEdit = NULL;
	m_hmodRichEdit = NULL;
	m_hWndTrace = NULL;
}
//------------------------------------------------------------------------
CTrace::~CTrace()
{
}
//------------------------------------------------------------------------
// ֱ�ӵ�����Ϣ��ʾ
BOOL CTrace::FKShowTrace(const wchar_t* szMsg, COLORREF color, int nTextHeight, 
						 DWORD dwStyle, BOOL bLn, BOOL bShowTime)
{
	if (m_dwOutputStyle == 0)
		return FALSE;

	TRACE_MSG tm;
	// �ַ���̫��,�ж�!
	if(wcslen(szMsg) * 2 >= MAX_TRACE_MSG_LEN-1)
	{
		wchar_t* pStr = new wchar_t[MAX_TRACE_MSG_LEN];
		memcpy(pStr, szMsg, (MAX_TRACE_MSG_LEN-1) * 2);
		*(pStr + MAX_TRACE_MSG_LEN-1) = 0;
		tm = SetMsg(pStr, color, nTextHeight, dwStyle, bLn, bShowTime);
		delete pStr;
		pStr = NULL;
	}
	else
		tm = SetMsg(szMsg, color, nTextHeight, dwStyle, bLn, bShowTime);

	if (bLn)
		tm.str += L"\r\n";

	// �����VC��Output������
	if (m_dwOutputStyle & OS_OUTPUTDEBUGSTRING)
		ShowToOutput(tm);

	// д���ļ�
	if (m_dwOutputStyle & OS_FILE)
		ShowToFile(tm);

	// �������Ӧ�ó��򴰿��е�RichEdit�ؼ���
	if (m_dwOutputStyle & OS_RICHEDIT)
		ShowToRichEdit(m_hRichEdit, tm);

	// ������Դ������е�RichEdit�ؼ���
	if (m_dwOutputStyle & OS_WINDOW)
		ShowToRichEdit(m_hMyRichEdit, tm);

	return TRUE;
}
//------------------------------------------------------------------------
// �̰߳�ȫ������Ϣ��ʾ(����OS_OUTPUTDEBUGSTRING��OS_FILE��ͬFKShowTrace())
BOOL CTrace::FKThreadSafeShowTrace(const wchar_t* szMsg, COLORREF color, int nTextHeight, 
								   DWORD dwStyle, BOOL bLn, BOOL bShowTime)
{
	if (m_dwOutputStyle == 0)
		return FALSE;

	TRACE_MSG tm;

	// �ַ���̫��,�ж�!
	if(wcslen(szMsg) * 2 >= MAX_TRACE_MSG_LEN -1)
	{
		wchar_t* pStr = new wchar_t[MAX_TRACE_MSG_LEN];
		memcpy(pStr, szMsg, (MAX_TRACE_MSG_LEN-1)*2);
		*(pStr + MAX_TRACE_MSG_LEN-1) = 0;
		tm = SetMsg(pStr, color, nTextHeight, dwStyle, bLn, bShowTime);
		delete pStr;
		pStr = NULL;
	}
	else
		tm = SetMsg(szMsg, color, nTextHeight, dwStyle, bLn, bShowTime);

	// ������ⴰ�ھ����Ϊ�գ���ôֻ����������ļ���VC�����������
	if (m_hWndTrace == NULL && m_hWndMain == NULL)
	{
		FKShowTrace(tm.str.c_str(), tm.crColor, tm.nHeight, tm.dwStyle, tm.bLn, tm.bShowTime);
		return TRUE;
	}

	m_mtxQueue.Lock();

	if(m_MessageQueue.size() >= MAX_THREAD_MESSAGE) // ��Ϣ������
	{
		TRACE_MSG tm1 = m_MessageQueue.front();
		m_MessageQueue.pop(); // ������һ����Ϣ
		m_MessageQueue.push(tm); // ѹ�����ڵ���Ϣ

		//char str[MAX_TRACE_MSG_LEN];
		if (tm1.bLn)
			tm1.str += L"\r\n";
		tm1.crColor = TC_WARNING;

		// �������Ӧ�ó��򴰿��е�RichEdit�ؼ���
		if (m_dwOutputStyle & OS_RICHEDIT)
			ShowToRichEdit(m_hRichEdit, tm1, TRUE);

		// ������Դ������е�RichEdit�ؼ���
		if (m_dwOutputStyle & OS_WINDOW)
			ShowToRichEdit(m_hMyRichEdit, tm1, TRUE);
	}
	else
	{
		m_MessageQueue.push(tm);

		// �����ⴰ�ڷ�����Ϣ
		if (m_hWndTrace)
			PostMessage(m_hWndTrace, WM_THREAD_TRACE_EX, 0, 0);
		else if (m_hWndMain)
			PostMessage(m_hWndMain, WM_THREAD_TRACE_EX, 0, 0);
	}

	//if (tm.bLn)
	//	tm.str += L"\r\n";
	// �����VC��Output������
	if (m_dwOutputStyle & OS_OUTPUTDEBUGSTRING)
		ShowToOutput(tm);

	// д���ļ�
	if (m_dwOutputStyle & OS_FILE)
		ShowToFile(tm);

	m_mtxQueue.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
// �̰߳�ȫ�����Ϣ�����е���Ϣ
BOOL CTrace::ThreadSafeGetTraceMsg()
{
	TRACE_MSG tm;
	m_mtxQueue.Lock();
	if(m_MessageQueue.empty())
		m_mtxQueue.Unlock();
	else
	{
		tm = m_MessageQueue.front();
		m_MessageQueue.pop();
		m_mtxQueue.Unlock();
	}

	if (m_dwOutputStyle == 0)
		return FALSE;

	if (tm.bLn)
		tm.str += _T("\r\n");
	// �������Ӧ�ó��򴰿��е�RichEdit�ؼ���
	if (m_dwOutputStyle & OS_RICHEDIT)
		ShowToRichEdit(m_hRichEdit, tm, TRUE);

	// ������Դ������е�RichEdit�ؼ���
	if (m_dwOutputStyle & OS_WINDOW)
		ShowToRichEdit(m_hMyRichEdit, tm, TRUE);

	return TRUE;
}
//------------------------------------------------------------------------
// �̰߳�ȫ��ȡ��Ϣ�����е���Ϣ����������ʾ
BOOL CTrace::ThreadSafeGetTraceMsgContext(wchar_t* szBuffer, int nSize, BOOL bRemoveMsg)
{
	if (!szBuffer) return FALSE;
	TRACE_MSG tm;
	m_mtxQueue.Lock();
	if(m_MessageQueue.empty())
	{
		m_mtxQueue.Unlock();
		return FALSE;
	}
	else
	{
		tm = m_MessageQueue.front();
		if (bRemoveMsg)
			m_MessageQueue.pop();
		m_mtxQueue.Unlock();
		if (tm.bLn)
			tm.str += L"\r\n";
		lstrcpyn(szBuffer, tm.str.c_str(), nSize);
	}

	return TRUE;
}
//------------------------------------------------------------------------
// ��̬�������������Ϣ���͵Ľӿڣ������ڳ������й����д򿪻�ر��ļ�Trace
BOOL CTrace::SetOutputFile(bool bOutFile, const wchar_t* szFileName)
{
	// ����������ļ�ʱ������ָ���ļ���
	if(bOutFile == TRUE && !szFileName)
	{
		return false;
	}

	// ��Trace���ļ�
	if(bOutFile == TRUE)
	{
		// ���ԭ���Ѿ������ļ������Ƚ�ԭ�ļ��ر�
		if(m_hFile)
		{
			fclose(m_hFile);
			m_hFile = 0;
		}

		// �������ļ�
		if(!CreateTraceFile(szFileName))
			return false;

		m_dwOutputStyle |= OS_FILE;
	}
	else
	{
		// ����д򿪵��ļ����������ر�
		if(m_hFile)
		{
			fclose(m_hFile);
			m_hFile = 0;
		}

		if(m_dwOutputStyle & OS_FILE)
		{
			m_dwOutputStyle &= (~OS_FILE);
		}
	}
	return true;
}
//------------------------------------------------------------------------
// �ͷ�(�����Դ����ڵ�����)
BOOL CTrace::Release()
{
	// �ͷ��ļ�
	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}

	if (m_pFileName)
	{
		delete m_pFileName;
		m_pFileName = NULL;
	}

	if (m_hWndTrace)
	{
		::DestroyWindow(m_hWndTrace);
		m_hWndTrace = NULL;
	}

	// �ͷ�riched32.dll
	if (m_hmodRichEdit)
	{
		::FreeLibrary(m_hmodRichEdit);
		m_hmodRichEdit = NULL;
	}

#ifdef _DEBUG
	OutputDebugString(L"�ͷ�FKTrace���\r\n");
#endif

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
// ���õ���ѡ��
BOOL CTrace::SetOption(DWORD dwOutputStyle)
{
	m_dwOutputStyle = dwOutputStyle;
	return m_dwOutputStyle != 0;
}
//------------------------------------------------------------------------
// ��ʼ��
BOOL CTrace::Init(HWND hWndMain, HWND hRichEdit, const wchar_t* szFileName, HINSTANCE hInst, BOOL bExitAll)
{
#ifdef _DEBUG
	BOOL bCreateFile = FALSE, bCreateWindow = FALSE;
#endif

	m_bExitAll = bExitAll;
	m_hWndMain = hWndMain;	// ��Ӧ�ó��򴰿�

	// ����������
	if (!m_mtxPrint.Create())
		return FALSE;

	// ����������
	if (!m_mtxQueue.Create())
		return FALSE;

	// �����յĵ����ļ�
	if (szFileName != NULL && (m_dwOutputStyle & OS_FILE))
#ifdef _DEBUG
		bCreateFile = 
#endif
		CreateTraceFile(szFileName);

	if (hWndMain != NULL && hRichEdit != NULL && (m_dwOutputStyle & OS_RICHEDIT))
		m_hRichEdit = hRichEdit;

	if (hInst != NULL && (m_dwOutputStyle & OS_WINDOW))
#ifdef _DEBUG
		bCreateWindow = 
#endif
		CreateTraceWindow(hInst);

	return TRUE;
}
//------------------------------------------------------------------------
// �����ڲ��Դ�����
BOOL CTrace::CreateTraceWindow(HINSTANCE hInst)
{
	LRESULT rs;
	static BOOL bIsCreated = FALSE;
	if (bIsCreated)
		return TRUE;

	m_hmodRichEdit = ::LoadLibrary(L"RICHED32.DLL");
	assert(m_hmodRichEdit != NULL);

	WNDCLASS            wc;
	wc.style		=	CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc	=	(WNDPROC)TraceWndProc;
	wc.cbClsExtra	=	0;
	wc.cbWndExtra	=	0;
	wc.hInstance	=	hInst;
	wc.hIcon		=	::LoadIcon(hInst,L"WS_ICON");
	wc.hCursor		=	::LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=	(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName	=	NULL;
	wc.lpszClassName=	L"TRACE_WINDOW";
	RegisterClass(&wc);
	wchar_t szTitle[MAX_PATH];
	wsprintf(szTitle, L"��FK���Դ��ڡ� �汾�� = %d", VERSION_QUERY_TRACE);

	RECT rcMain;
	SetRectEmpty(&rcMain);
	if (m_hWndMain != NULL)
		::GetWindowRect(m_hWndMain, &rcMain);

	m_hWndTrace = ::CreateWindow(L"TRACE_WINDOW", szTitle, 
		WS_OVERLAPPEDWINDOW, 
		rcMain.right, rcMain.top, 420, 768 + 34, 0, 0, hInst, 0);
	assert(m_hWndTrace != NULL);
	m_hMyRichEdit = ::CreateWindowW(L"RICHEDIT", L"",
		WS_CHILD |ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP,
		rcMain.right, rcMain.top, 420, 768 + 34,m_hWndTrace,0,hInst,0);
	assert(m_hMyRichEdit != NULL);

	assert(m_hWndTrace != NULL);
	::SetWindowLong(m_hWndTrace, GWL_USERDATA, (LONG)this);
	::ShowWindow(m_hWndTrace, SW_SHOWNORMAL);
	if (m_hWndMain != NULL)
	{
		::GetWindowRect(m_hWndTrace, &m_rcClient);

		::SetWindowPos(m_hWndTrace, HWND_TOP,rcMain.right, rcMain.top, m_rcClient.right - m_rcClient.left, 
			m_rcClient.bottom -  m_rcClient.top, SWP_SHOWWINDOW);
		::SetActiveWindow(m_hWndMain);
	}

	assert(::IsWindow(m_hMyRichEdit));

	CHARFORMAT2 cf;
	cf.cbSize = sizeof (CHARFORMAT);
	cf.dwMask = CFM_FACE | CFM_SIZE;
	cf.bCharSet = GB2312_CHARSET;
	cf.yHeight = 9*20;
	wsprintf(cf.szFaceName, L"MS Sans Serif");

	rs = ::SendMessage(m_hMyRichEdit, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	rs = ::SendMessage(m_hMyRichEdit, EM_SETBKGNDCOLOR, FALSE, RGB(137,202,157) );
	long lEventMask = (long)::SendMessage(m_hMyRichEdit, EM_GETEVENTMASK, 0, 0L);
	rs = ::SendMessage(m_hMyRichEdit, EM_SETEVENTMASK, 0, lEventMask | ENM_LINK);
	rs = ::SendMessage(m_hMyRichEdit, EM_AUTOURLDETECT, TRUE,0);

	bIsCreated = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
// �����洢������Ϣ���ļ�
BOOL CTrace::CreateTraceFile(const wchar_t* szTraceFile)
{
	// ��¼�ļ��Ƿ��Ѿ���
	static BOOL bIsCreated = FALSE;
	if (bIsCreated)
		return TRUE;

	// ��¼�ļ���
	int nLen = lstrlen(szTraceFile);
	m_pFileName = new wchar_t[nLen+1];
	lstrcpy(m_pFileName, szTraceFile);

	// ����������ļ�����
	m_hFile = _wfopen( szTraceFile,L"a" );
	if (m_hFile == NULL)
	{
		OutputDebugString(	L"����FKTrace����������Ϣ�ļ������ļ�ʧ�ܣ�\r\n");
		return FALSE;
	}

	bIsCreated = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
// ��RichEdit�������Ϣ
BOOL CTrace::AddToRichEdit(HWND hWnd, TRACE_MSG tm)
{
	if (hWnd == NULL || !::IsWindow(hWnd))
		return FALSE;

	CHARRANGE crOld;
	::SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&crOld);

	int nLen = (int)::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
	int nStrLen = tm.str.size();
	CHARRANGE cr;
	if(nLen + nStrLen > MAX_RICHEDIT_MESSAGE_LEN)
	{
		cr.cpMin = 0;
		cr.cpMax = nLen + nStrLen - MAX_RICHEDIT_MESSAGE_LEN;
		::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);
		::SendMessage(hWnd, EM_REPLACESEL, (WPARAM)0, (LPARAM)"");
		nLen = (int)::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
	}

	cr.cpMin = nLen;
	cr.cpMax = nLen;
	::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);

	CHARFORMAT2 cf;
	memset(&cf,0,sizeof(CHARFORMAT2));
	cf.dwMask = CFM_COLOR | CFM_CHARSET | CFM_SIZE 
		| CFM_BOLD | CFM_ITALIC | CFM_STRIKEOUT |  CFM_UNDERLINE | CFM_LINK | CFM_SHADOW;
	cf.dwEffects = tm.dwStyle;
	cf.bCharSet = GB2312_CHARSET;
	cf.crTextColor = tm.crColor;
	cf.yHeight = tm.nHeight * 20;
	cf.cbSize = sizeof(CHARFORMAT);
	::SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	((void)::SendMessage((hWnd), EM_REPLACESEL, 0L, (LPARAM)(LPCTSTR)(tm.str.c_str())));

	if(crOld.cpMax > crOld.cpMin)
	{
		::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&crOld);
	}

	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	::GetScrollInfo(hWnd, SB_VERT, &ScrollInfo);

	int nTotalLine = (int)::SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
	int nUpLine = 0 ;
	if(nTotalLine > 0 && ScrollInfo.nMax > 0 && ScrollInfo.nMax / nTotalLine > 0)
		nUpLine = (ScrollInfo.nMax - ScrollInfo.nPos - (ScrollInfo.nPage - 1)) / (ScrollInfo.nMax / nTotalLine);
	if(nUpLine > 0)
		::SendMessage(hWnd, EM_LINESCROLL, 0, nUpLine);

	return TRUE;
}
//------------------------------------------------------------------------
// ��Ϣ���
TRACE_MSG CTrace::SetMsg(const wchar_t* str, COLORREF color, int nHeight, DWORD dwStyle, BOOL bLn, BOOL bShowTime)
{
	TRACE_MSG tm;
	tm.str	= str;
	tm.crColor = color;
	tm.nHeight = nHeight;
	tm.dwStyle = dwStyle;
	tm.bLn = bLn;
	tm.bShowTime = bShowTime;
	return tm;
}
//------------------------------------------------------------------------
// д�ļ�
BOOL CTrace::WriteString(const wchar_t* szMsg, int nSize)
{
	if(m_hFile == NULL)
	{
		return FALSE;
	}

	// setlocale(LC_ALL, "chinese-simplified"); 

	fwprintf_s( m_hFile, szMsg );
	fflush(m_hFile);
	return TRUE;
}
//------------------------------------------------------------------------
// �ߴ�ı�
BOOL CTrace::OnSize()
{
	::GetClientRect(m_hWndTrace, &m_rcClient);
	::ClientToScreen(m_hWndTrace, (POINT*)&m_rcClient.left);
	::ClientToScreen(m_hWndTrace, (POINT*)&m_rcClient.right);

	::SetWindowPos(m_hMyRichEdit, HWND_TOP,0,0, m_rcClient.right - m_rcClient.left, 
		m_rcClient.bottom -  m_rcClient.top, SWP_SHOWWINDOW);
	return TRUE;
}
//------------------------------------------------------------------------
// ��ȡ����ʱ���ַ���
BOOL CTrace::GetDateTimeString(wchar_t* str)
{
	time_t	long_time;
	struct tm* local_time;
	time(&long_time);
	local_time = localtime(&long_time);

	wsprintf(str, L"[Day:%02d %02d - Time:%02d:%02d:%02d]", local_time->tm_mon+1, local_time->tm_mday,
		local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	return TRUE;
}
//------------------------------------------------------------------------
// ��ʾ��VC�ĵ��Դ���
BOOL CTrace::ShowToOutput(TRACE_MSG& tm)
{
	wchar_t strDateTime[48];
	if (tm.bShowTime)
	{
		GetDateTimeString(strDateTime);
		OutputDebugString(strDateTime);
	}

	OutputDebugString(tm.str.c_str());

	return TRUE;
}
//------------------------------------------------------------------------
// ��ʾ���ļ�
BOOL CTrace::ShowToFile(TRACE_MSG& tm)
{
	wchar_t strDateTime[48];
	if (tm.bShowTime)
	{
		GetDateTimeString(strDateTime);
		WriteString(strDateTime, wcslen(strDateTime));
	}

	WriteString(tm.str.c_str(), tm.str.size());

	return TRUE;
}
//------------------------------------------------------------------------
// ��ʾ��RichEdit��
BOOL CTrace::ShowToRichEdit(HWND hWnd, TRACE_MSG& tm, BOOL bUseThread)
{
	if (hWnd == NULL)
		return FALSE;

	if (bUseThread)
		m_mtxPrint.Lock();

	wchar_t strDateTime[48];
	if (tm.bShowTime)
	{
		GetDateTimeString(strDateTime);
		AddToRichEdit(hWnd, SetMsg(strDateTime, 0));
	}

	if( tm.bLn )
		tm.str += L"\r\n";

	AddToRichEdit(hWnd, tm);

	if (bUseThread)
		m_mtxPrint.Unlock();

	return TRUE;
}
//------------------------------------------------------------------------
// �����˳�
BOOL CTrace::IsExitAll()
{
	return m_bExitAll;
}
//------------------------------------------------------------------------
// �Ի�����Ϣ�������
LRESULT CALLBACK TraceWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_DESTROY:
		if (((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->IsExitAll())
			PostQuitMessage(0);
		break;
	case WM_CLOSE:
		if (!((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->IsExitAll())
		{
			::ShowWindow(hWnd, SW_HIDE);
			return TRUE;
		}
		break;
	case WM_SIZE:
		((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->OnSize();
		return TRUE;
	case WM_THREAD_TRACE_EX:
		((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->ThreadSafeGetTraceMsg();
		if (::IsIconic(hWnd)) // ��С����ָ�
			::ShowWindow(hWnd, SW_RESTORE);
		else
			::ShowWindow(hWnd, SW_SHOW);
		break;
	case WM_QUERYOPEN:
		::ShowWindow(hWnd, SW_RESTORE);
		break;
	default:
		break;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//------------------------------------------------------------------------
BOOL CFKTrace::CreateTraceObject(DWORD dwExpectVersion, 
					DWORD dwOutputStyle, HWND hWndMain, 
					HWND hRichEdit, const wchar_t* szFileName, 
					HINSTANCE hInst, BOOL bExitAll, 
					ITrace** ppTrace)
{
	// �汾У��
	// �����жϿͻ������dwExpectVersion�еĸ�λWord
	if (HIWORD(dwExpectVersion) != HIWORD(VERSION_QUERY_TRACE))
		return FALSE;		// ��Word��ͬ

	// Ȼ���ж�dwExpectVersion�еĵ�λWord
	if (LOWORD(dwExpectVersion) > LOWORD(VERSION_QUERY_TRACE))
		return FALSE;		// �ͻ�Ĭ�ϵĵĵ�Word̫��

	// ��֤OS_RICHEDIT����Ƿ�������Ч
	if (hWndMain == NULL || hRichEdit == NULL || !(dwOutputStyle & OS_RICHEDIT))
	{
		hRichEdit = NULL;
		if (dwOutputStyle & OS_RICHEDIT)
			dwOutputStyle &= ~OS_RICHEDIT;
	}

	// ��֤OS_FILE����Ƿ�������Ч
	if (szFileName == NULL || !(dwOutputStyle & OS_FILE) || *szFileName == 0)
	{
		szFileName = NULL;
		if (dwOutputStyle & OS_FILE)
			dwOutputStyle &= ~OS_FILE;
	}

	CTrace* pTrace = new CTrace();
	if (pTrace == NULL)
		return FALSE;

	if (hInst == NULL)
		hInst = ::GetModuleHandle(NULL);

	pTrace->SetOption(dwOutputStyle);
	pTrace->Init(hWndMain, hRichEdit, szFileName, hInst, bExitAll);

	*ppTrace = static_cast<ITrace*>(pTrace);
	return *ppTrace != NULL;
}
//------------------------------------------------------------------------