/**
*	created:		2013-5-10   6:23
*	filename: 		FKLinkProxyWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKClientSocket.h"
#include "FKFunc.h"
#include <atlbase.h>
#include <atlwin.h>
//------------------------------------------------------------------------
#define WM_LINK_MSG	(WM_USER + 123)
//------------------------------------------------------------------------
class CLinkProxyWnd : public CWindowImpl<CLinkProxyWnd>,public IClientSocketSink,public ILinkProxy
{
public:
	void Close();
	CLinkProxyWnd();
	DECLARE_WND_CLASS(NULL)
	BEGIN_MSG_MAP(CLinkProxyWnd)
		MESSAGE_HANDLER(WM_LINK_MSG, OnLinkMsg)
	END_MSG_MAP()

	BOOL Create(IThreadRoute *pThreadRoute,ITrace* pTrace,BOOL bSyncByWnd,BOOL bInSameThread);

	virtual BOOL AddSocket(DWORD dwSocketID,IClientSocketSink *pSink,IClientSocket** ppClientSocket);
	virtual BOOL RemoveSocket(IClientSocket* pClientSocket);
	virtual void Release()
	{
		Close();
		if(m_hWnd)
			DestroyWindow();
		delete this;
	}

private:
	BOOL RemoveSocket(DWORD dwSocketIndex);
	BOOL IsValidSocketIndex(DWORD dwSocketIndex)
	{
		if(dwSocketIndex >= MAX_CLIENT_SOCKET)
			return FALSE;
		return m_SocketBuf[dwSocketIndex].dwSocketID != INVALID_SOCKET_ID;
	}


	BOOL	CreateWnd();
	LRESULT OnLinkMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// ���ӷ�����ʧ��
	virtual void OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode);
	// ���ӽ���
	virtual void OnConnectOK(DWORD dwSocketID);
	// �յ����ݰ�
	// ע��	����������ݰ����߳���IThreadRoute����ͬһ�̣߳�
	//		����һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
	virtual void OnDataArrived(DWORD dwSocketID);

	// ��ǰ���ӶϿ�
	virtual void OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode);
public:
	enum LinkMsgType
	{
		linkConnectError,linkConnectOK,linkDataArrived,linkConnectClose,linkDataSending
	};
	struct LINK_NOTE
	{
		LINK_NOTE(DWORD	dwSocketIndex,LPCSTR szData,DWORD dwDataLen,DWORD dwErrorCode)
		{
			if(szData)
			{
				this->szData = MyAllocMemory(dwDataLen);
				if(this->szData)
					memcpy(this->szData,szData,dwDataLen);
			}
			else
				this->szData = NULL;
			this->dwSocketIndex = dwSocketIndex;
			this->dwDataLen = dwDataLen;
			this->dwErrorCode = dwErrorCode;
		}
		~LINK_NOTE()
		{
			if(szData)
				delete szData;
		}
		DWORD		dwSocketIndex;
		char*		szData;			// �������Ӵ����������ַ���������ͨѶ�������������ݰ�
		DWORD		dwDataLen;		// ���ݰ�����
		DWORD		dwErrorCode;	//
	};
private:
	struct SocketInfo
	{
		SocketInfo()
		{
			pClientSocket = NULL;
			Close();
		}
		DWORD				dwSocketID;
		IClientSocketSink*	pSink;
		IClientSocket*		pClientSocket;
		void Close()
		{
			dwSocketID = INVALID_SOCKET_ID;
			pSink = NULL;
			if(pClientSocket)
			{
				if(pClientSocket->IsConnected())
					pClientSocket->CloseConnect();	// ��Ӧ�ø���ɾ��
				pClientSocket = NULL;
			}
		}
		BOOL IsValid()
		{
			return dwSocketID != INVALID_SOCKET_ID;
		}
	};

	SocketInfo		m_SocketBuf[MAX_CLIENT_SOCKET];
	CCriticalSectionLock	m_LockSocketBuf;	// ɾ��m_SocketBuf�е�һ���ͨѶ�߳�IClientSocketSink�¼���Ҫ�������
	int				m_nSocketCount;
	int				m_nBufLoopIndex;			// �������µ�socketʱ��ѭ������m_SocketBuf������
												// �������ĺô��ǣ��µ�socket��������ʹ��һ���ϵ�socket��
	IThreadRoute*	m_pThreadRoute;
	ITrace*			m_pTrace;

	BOOL			m_bInSameThread;
	BOOL			m_bSyncByWnd;
public:
};
//------------------------------------------------------------------------