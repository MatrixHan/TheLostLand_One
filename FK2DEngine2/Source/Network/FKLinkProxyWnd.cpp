/**
*	created:		2013-5-10   6:27
*	filename: 		FKLinkProxyWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
 * ���ܣ�������IClientSocket����
 *		 �������Ϊ���̷߳�ʽ������ѡ����һ����ʱ��wnd������Ϣͬ����ͨ�����л��巽ʽʵ��ͬ��
 * ע�⣺��Ϊ�����ӹ�������������ÿ�����ӵļ��ܺͷ����
 * ������
 *		1����������ͨѶʱ��ͨѶ�̺߳����߳�����һ��ģ���ȡֱ�ӻص���ʽ�ɻ�ȡ�������
 *		2�����ͨѶ�̺߳����̲߳�����ͬһ���̣߳�����Ҫ����CLinkProxyWndʵ��ͬ������
 * ˵����1��IClientSink�ļ����ӿ�������IThreadRoute�б����ã���AddSocket()��RemoveSocket()
 *		 ��������һ���߳��б����ã����Կ��ܴ��ڷ���m_SocketBuf���߳�ͬ�����⣬�ⲿ����
 *		 ���뱣֤RemoveSocketʱû�����socket��ص��¼�����������socket�Ѿ��رյ��ˣ�
 *		 ���Ǵ�Ч�ʽǶȿ��ǵģ����ض�ÿ���¼���Ӧ�����ж�m_SocketBuf����
 *		2���û�ִ��RemoveSocket()�ر����ӣ�������IClientSocket->CloseConnect()�����쳣���ӶϿ�ʱCLinkProxyWnd�Զ����RemoveSocket()����
 *		3���ݲ�ʵ�ֵ�Ч�Ķ������ݰ����淽ʽ
 * �޸ģ�
 *		1��ɾ������ʱ��Ӧ�ò㸺ִ��pClientSocket->Release()����
 */
//------------------------------------------------------------------------
#include <atlbase.h>
#include <atlwin.h>
#include "../../Include/Network/FKThreadRoute.h"
#include "../../Include/Network/FKClientSocket.h"
#include "../../Include/Network/FKCClientSocket.h"
#include "../../Include/Network/FKLinkProxyWnd.h"
#include "../../Include/Network/FKFunc.h"
#include "../../Include/ResMgr/ResMgrCommon.h"
//------------------------------------------------------------------------
#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
//------------------------------------------------------------------------
CLinkProxyWnd::CLinkProxyWnd()
{
	m_nBufLoopIndex = 0;
	m_nSocketCount = 0;

	m_pThreadRoute = NULL;
	m_pTrace = NULL;

	m_bInSameThread = FALSE;
	m_bSyncByWnd = TRUE;
}
//------------------------------------------------------------------------
// ���̶߳��з�ʽ�ݲ�֧��
extern "C" BOOL CreateLinkProxyWnd(IThreadRoute *pThreadRoute,ITrace* pTrace,BOOL bSyncByWnd,BOOL bInSameThread,ILinkProxy** ppLinkProxy)
{
	if(!bInSameThread && !bSyncByWnd)
	{
		TraceLn("Error : CreateLinkProxyWnd() : this thread mode is not be supported now!");
		return FALSE;
	}
	try
	{
		if(pThreadRoute == NULL)
			throw "param error";

		CLinkProxyWnd* pProxyWnd = new CLinkProxyWnd();
		if(pProxyWnd == NULL)
			throw "No Memory";
		if(!pProxyWnd->Create(pThreadRoute,pTrace,bSyncByWnd,bInSameThread))
			throw "Create wnd failed";
		*ppLinkProxy = static_cast<ILinkProxy*>(pProxyWnd);
		return TRUE;
	}
	catch(LPCSTR szMsg)
	{
		TraceLn("Error : CreateLinkProxy() : %s",szMsg);
		return FALSE;
	}
}
//------------------------------------------------------------------------
BOOL CLinkProxyWnd::CreateWnd()
{
	//RECT rect = {0,0,400,400};
	//m_hWnd = CWindowImpl<CLinkProxyWnd>::Create(NULL,rect,NULL,WS_POPUP);
	//return m_hWnd != NULL;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CLinkProxyWnd::Create(IThreadRoute *pThreadRoute,ITrace* pTrace,BOOL bSyncByWnd,BOOL bInSameThread)
{
#ifdef _DEBUG
	// ��ʱʼ�տ����� FreeKnight
	// m_LockSocketBuf.SetNoneLock(bInSameThread);
#endif

	m_pThreadRoute = pThreadRoute;
	m_pTrace = pTrace;
	m_bInSameThread = bInSameThread;
	m_bSyncByWnd = bSyncByWnd;
	if(bSyncByWnd)
		return CreateWnd();
	return TRUE;
}
//------------------------------------------------------------------------
// ע����ͨѶ�̻߳ص�����wnd����ͬһ���߳�
// ���룺dwSocketID	����CLinkProxyWnd�ж���Ϊsocket��m_SocketBuf�е�����
// ע��
//		Ӧ�ò��յ�����Ϣ�Ժ���Ҫ����CLinkProxyWnd::RemoveSocket()ɾ����socket����
void CLinkProxyWnd::OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode)
{
	m_LockSocketBuf.Lock();
	if(!IsValidSocketIndex(dwSocketID))
	{
		m_LockSocketBuf.Unlock();
		DebugMsgLn(L"Error : CLinkProxyWnd::OnConnectError() : invalid socket index = %d",dwSocketID);
		return;
	}
	SocketInfo& skt = m_SocketBuf[dwSocketID];
	if(m_bInSameThread)
	{
		skt.pSink->OnConnectError(skt.dwSocketID,szMsg,dwErrorCode);
	}
	else if(m_bSyncByWnd)
	{
		LINK_NOTE *pNode = new LINK_NOTE(dwSocketID,szMsg,strlen(szMsg) + 1,dwErrorCode);
		if(pNode)
			::PostMessage(m_hWnd,WM_LINK_MSG,linkConnectError,(LPARAM)pNode);
	}
	else
	{
		// not supported now
		//.......
	}
	m_LockSocketBuf.Unlock();
}
//------------------------------------------------------------------------
// ע����ͨѶ�̻߳ص�����wnd����ͬһ���߳�
// ���룺dwSocketID	����CLinkProxyWnd�ж���Ϊsocket��m_SocketBuf�е�����
void CLinkProxyWnd::OnConnectOK(DWORD dwSocketID)
{
	m_LockSocketBuf.Lock();
	if(!IsValidSocketIndex(dwSocketID))
	{
		m_LockSocketBuf.Unlock();
		DebugMsgLn(L"Error : CLinkProxyWnd::OnConnectOK() : invalid socket index = %d",dwSocketID);
		return;
	}
	SocketInfo& skt = m_SocketBuf[dwSocketID];
	if(m_bInSameThread)
		skt.pSink->OnConnectOK(skt.dwSocketID);
	else if(m_bSyncByWnd)
	{
		LINK_NOTE *pNode = new LINK_NOTE(dwSocketID,NULL,0,0);
		if(pNode)
			::PostMessage(m_hWnd,WM_LINK_MSG,linkConnectOK,(LPARAM)pNode);
	}
	m_LockSocketBuf.Unlock();
}
//------------------------------------------------------------------------
// ע����ͨѶ�̻߳ص�����wnd����ͬһ���߳�
// ���룺dwSocketID	����CLinkProxyWnd�ж���Ϊsocket��m_SocketBuf�е�����
void CLinkProxyWnd::OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode)
{
	m_LockSocketBuf.Lock();
	if(!IsValidSocketIndex(dwSocketID))
	{
		DebugMsgLn(L"Error : CLinkProxyWnd::OnConnectClose() : invalid socket index = %d",dwSocketID);
		m_LockSocketBuf.Unlock();
		return;
	}
	SocketInfo& skt = m_SocketBuf[dwSocketID];
	if(m_bInSameThread)
	{
		skt.pSink->OnConnectClose(skt.dwSocketID,szMsg,dwErrorCode);
	}
	else if(m_bSyncByWnd)
	{
		LINK_NOTE *pNode = new LINK_NOTE(dwSocketID,szMsg,strlen(szMsg) + 1,dwErrorCode);
		if(pNode)
			::PostMessage(m_hWnd,WM_LINK_MSG,linkConnectClose,(WPARAM)pNode);
	}
	m_LockSocketBuf.Unlock();
}
//------------------------------------------------------------------------
// ע����ͨѶ�̻߳ص�����wnd����ͬһ���߳�
// ���룺dwSocketID	����CLinkProxyWnd�ж���Ϊsocket��m_SocketBuf�е�����
void CLinkProxyWnd::OnDataArrived(DWORD dwSocketID)
{
	m_LockSocketBuf.Lock();
	if(!IsValidSocketIndex(dwSocketID))
	{
		DebugMsgLn(L"Error : CLinkProxyWnd::OnDataArrived() : invalid socket index = %d",dwSocketID);
		m_LockSocketBuf.Unlock();
		return;
	}
	SocketInfo& skt = m_SocketBuf[dwSocketID];
	if(m_bInSameThread)
		skt.pSink->OnDataArrived(skt.dwSocketID);
	else if(m_bSyncByWnd)
	{
		::PostMessage(m_hWnd,WM_LINK_MSG,linkDataArrived,(WPARAM)dwSocketID);
	}
	m_LockSocketBuf.Unlock();
}
//------------------------------------------------------------------------
BOOL CLinkProxyWnd::AddSocket(DWORD dwSocketID,IClientSocketSink *pSink,IClientSocket** ppClientSocket)
{
	Assert(pSink != NULL);
	Assert(ppClientSocket != NULL);
	m_LockSocketBuf.Lock();
	if(m_nSocketCount >= MAX_CLIENT_SOCKET)
	{
		m_LockSocketBuf.Unlock();
		DebugMsgLn(L"Error : CLinkProxyWnd::AddSocket() ��too much sockets");
		return FALSE;
	}
	for(DWORD i = 0; i < MAX_CLIENT_SOCKET; i++)
	{
		if(m_SocketBuf[i].dwSocketID == dwSocketID)
		{
			TraceLn("Error : CLinkProxyWnd::AddSocket() ��find same dwSocketID = %d",dwSocketID);
			return FALSE;
		}
	}
	// Ѱ��һ����λ
	DWORD dwIndex = 0;
	for(; dwIndex < MAX_CLIENT_SOCKET; dwIndex++)
	{
		if(m_SocketBuf[m_nBufLoopIndex].dwSocketID == INVALID_SOCKET_ID)
			break;
		m_nBufLoopIndex = (m_nBufLoopIndex + 1) % MAX_CLIENT_SOCKET;
	}
	if(dwIndex >= MAX_CLIENT_SOCKET)
	{
		m_LockSocketBuf.Unlock();
		DebugMsgLn(L"Error : CLinkProxyWnd::AddSocket() ��no free socket");
		return FALSE;
	}

	IClientSocket* pClientSocket = NULL;
	if(!CreateClientSocket(dwIndex,static_cast<IClientSocketSink*>(this),m_pThreadRoute,m_pTrace,m_bInSameThread,&pClientSocket))
	{
		m_LockSocketBuf.Unlock();
		DebugMsgLn(L"Error : CLinkProxyWnd::AddSocket() ��CreateClientSocket() failed");
		return FALSE;
	}
	
	SocketInfo& skt = m_SocketBuf[m_nBufLoopIndex];
	skt.dwSocketID = dwSocketID;
	skt.pClientSocket = pClientSocket;
	skt.pSink = pSink;
	m_nSocketCount++;
	*ppClientSocket = pClientSocket;

	m_nBufLoopIndex = (m_nBufLoopIndex + 1) % MAX_CLIENT_SOCKET;
	m_LockSocketBuf.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CLinkProxyWnd::RemoveSocket(DWORD dwSocketIndex)
{
	m_LockSocketBuf.Lock();
	if(!IsValidSocketIndex(dwSocketIndex))
	{
		m_LockSocketBuf.Unlock();
		DebugMsgLn(L"Error : CLinkProxyWnd::RemoveSocket() : invalid socket index = %d",dwSocketIndex);
		return FALSE;
	}
	m_SocketBuf[dwSocketIndex].Close();
	m_nSocketCount--;
	m_LockSocketBuf.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CLinkProxyWnd::RemoveSocket(IClientSocket *pClientSocket)
{
	m_LockSocketBuf.Lock();
	for(DWORD i = 0; i < MAX_CLIENT_SOCKET; i++)
		if(m_SocketBuf[i].IsValid() && m_SocketBuf[i].pClientSocket == pClientSocket)
		{
			m_SocketBuf[i].Close();
			m_nSocketCount--;
			m_LockSocketBuf.Unlock();
			return TRUE;
		}
	m_LockSocketBuf.Unlock();
	return FALSE;
}
//------------------------------------------------------------------------
// ���û���HWND�ķ�ʽͬ�����̣߳�ǰ����������ӵ�д�����Ϣѭ��
LRESULT CLinkProxyWnd::OnLinkMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	if(wParam == linkDataArrived)
	{
		SocketInfo& skt = m_SocketBuf[(DWORD)lParam];
		if(skt.IsValid())
		{
			if(skt.pClientSocket->IsConnected())
			{
				skt.pSink->OnDataArrived(skt.dwSocketID);
			}
			else
			{
			}
		}
		else
		{
			DebugMsgLn(L"Warn : CLinkProxyWnd::OnLinkMsg() : socket[%d] is invalid now",lParam);
		}
		return 0;
	}
	LINK_NOTE * pNote = (LINK_NOTE*)lParam;

	// ��ΪSocketInfo����ɾ�ڱ��߳�����ɣ����Բ��ؽ��л��⴦��
	SocketInfo& skt = m_SocketBuf[pNote->dwSocketIndex];
	if(skt.IsValid())
	{
		switch(wParam)
		{
			case linkDataSending:
				((CClientSocket *)(skt.pClientSocket))->OnFD_WRITE();
				break;
			case linkConnectOK:
				skt.pSink->OnConnectOK(skt.dwSocketID);
				break;
			case linkConnectError:
				skt.pSink->OnConnectError(skt.dwSocketID,pNote->szData,pNote->dwErrorCode);
				break;
			case linkConnectClose:
				if(skt.pClientSocket->IsConnected())
				{
					skt.pSink->OnConnectClose(skt.dwSocketID,pNote->szData,pNote->dwErrorCode);
					skt.pClientSocket->CloseConnect();	// ���ӶϿ�ʱ������������Դ����ΪΪ��֤Ӧ�ò���յ����ݣ�ClientSocket.cppû������������Դ
				}
				break;
			default:
				break;
		}
	}
	else
	{
		// ԭ�򣺵������Ѿ��Ͽ������Ӷ����Ѿ�����m_SocketBuf�У���ϵͳ��Ϣ�����п��ܻ������ӵĴ�����Ϣ
		DebugMsgLn(L"Warn : CLinkProxyWnd::OnLinkMsg() : socket[%d] is invalid now",pNote->dwSocketIndex);
	}
	if(pNote)
		delete pNote;
	return 0;
}
//------------------------------------------------------------------------
void CLinkProxyWnd::Close()
{
	for(DWORD i = 0; i < MAX_CLIENT_SOCKET; i++)
		if(m_SocketBuf[i].IsValid())
			m_SocketBuf[i].Close();
	m_nSocketCount = 0;
	if(m_hWnd)
		DestroyWindow();
}
//------------------------------------------------------------------------