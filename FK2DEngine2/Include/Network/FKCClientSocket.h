/**
*	created:		2013-5-10   2:59
*	filename: 		FKCClientSocket
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <winsock2.h>
#include "FKThreadRoute.h"
#include "FKClientSocket.h"
#include "FKAPIProxy.h"
#ifdef _DEBUG
#define __USE_MALLOC		// ���SGL STL�ڴ��й¶����
#endif
#include <list>
#include "FKFunc.h"
//------------------------------------------------------------------------
struct ITrace;
//------------------------------------------------------------------------
class CClientSocket : public IClientSocket,public IEventSink , public ITimerEventSink
{
	enum EnumSocketState
	{
		stateClosed = 0,			/*�����Ѿ��ر�*/
		stateReqConnect = 1			/*Ӧ�ò㷢�����ӣ���ͨѶ�̻߳�û�з�������*/,
		stateConnecting = 2			/*ͨѶ�������첽����*/,
		stateConnected = 3			/*���ӳɹ�*/,
		stateConnectClosing = 4		/*�������ڹر�*/
	};
public:
	CClientSocket();
	~CClientSocket();
public:
	BOOL	DoConnect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption);
	BOOL	StartupSocket();
	void	OnFD_WRITE();
	int		DoSend(WSABUF& wsabuf,DWORD& dwByteSent);
	void	ShowSocketError(char *szErrorMsg, int nErrorCode);
	void	ShowConnectError(char *szErrorMsg, int nErrorCode);
	BOOL	GetIP(char *szName,struct in_addr *pAddr);

	void	Close();
	BOOL	Create(DWORD dwSocketID,IClientSocketSink *pSink,IThreadRoute *pThreadRoute, ITrace* pTrace,BOOL bInSameThread);
public:
	// ��ǰͨ����ʶ
	virtual DWORD GetSocketID(){
		return m_dwSocketID;
	}

	// �������ӣ����յ�OnConnectOK()ʱ����ʾ���ӳɹ����յ�OnConnectError()ʱ��ʾ����ʧ��
	// ���û��߳��б�ִ��
	virtual BOOL Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption = NULL);

	// �رմ����ӣ��´ο����ٴε���Connect()��������
	// �û��߳��б�ִ��
	virtual BOOL CloseConnect();

	// ���������ͣ����ݷ���ͨʱ�Ž����ݷ��뷢�Ͷ��еȴ��´η���
	// �������ʧ�ܣ�Ӧ�����ر�����
	// ��IThreadRoute��ִ��
	virtual int Send(LPCSTR buf,int nLen);

	// ��������
	// ���û��߳��б�ִ��
	virtual BOOL Recv(int nBufLen,/*out*/char* buf,/*out*/int& nLen);


	// ��ѯ��ǰ�Ƿ�������״̬
	// ���û��߳��б�ִ��
	virtual BOOL IsConnected(){
		return m_ConnectState == stateConnected;
	}

	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData)
	{
		switch(dwCmd)
		{
		case IClientSocket::setMaxSendQueueCount:
			if(m_nMaxSendQueueCount <= 0 || m_nMaxSendQueueCount > 10000000)
				return FALSE;
			if(pData)
				*(int*)pData = m_nMaxSendQueueCount;
			m_nMaxSendQueueCount = dwParam;
			return TRUE;
		case IClientSocket::getMaxSendQueueCount:
			if(pData == NULL)
				return FALSE;
			*(int*)pData = m_nMaxSendQueueCount;
			return TRUE;
		case IClientSocket::getSocketHandle:
			if( pData == NULL )
				return FALSE;
			*(SOCKET *)pData = m_SocketClient;
			return TRUE;
		}
		return FALSE;
	}
	// �ͷŶ���
	virtual void Release(){
		Close();
		delete this;
	}

	virtual void GotoNextState(IClientSocketSink* pSink){
		m_pSink = pSink;
	}
public:
	virtual void	OnEvent(DWORD dwEventID);
	virtual DWORD	OnTimer(DWORD dwTimerID,DWORD dwParam);
private:
	void ClearSocketData();
	BOOL IsClosed(){
		return m_ConnectState == stateClosed;
	}
private:
	int						m_ConnectState;		// �Ƿ��Ѿ����ӣ�ע������״̬��ͬʱͨѶ�̺߳�Ӧ�ò��߳�ͬʱ�ı������
	DWORD					m_dwCommEventID;
	DWORD					m_dwTimerEventID;
	DWORD					m_dwConnectEventID;	// ���߳���
	SOCKET					m_SocketClient;
	HANDLE					m_hRefCommEvent;
	char					m_szServerAddress[MAX_PATH];
	WORD					m_wServerPort;
	NET_PROXY_OPTION		m_ProxyOption;
	DWORD					m_dwSocketID;
	IThreadRoute*			m_pThreadRoute;
	IClientSocketSink*		m_pSink;
	ITrace*					m_pTrace;
	HANDLE					m_hOverlapedEvent;

	std::list<WSABUF>		m_SendQueue;
	CCriticalSectionLock	m_LockClientSocketContext;	// �������̷߳��ʵ�ClientSocket��Դ���ô�������
	int						m_nMaxSendQueueCount;
	static	int				m_nWSAStartupRefCount;
};
//------------------------------------------------------------------------