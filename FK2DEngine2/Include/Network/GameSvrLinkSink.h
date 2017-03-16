/**
*	created:		2013-5-10   9:48
*	filename: 		GameSvrLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKClientSocket.h"
#include "FKThreadRoute.h"
#include "IFKAppNetworkLink.h"
#include "FKFunc.h"
#include <list>
#include <vector>
//------------------------------------------------------------------------
using std::list;
using std::vector;
//------------------------------------------------------------------------
struct IDataReceiver
{
	virtual BOOL OnReceived(const char* pData, int nLen) = 0;	
};
//------------------------------------------------------------------------
struct stReceiverInfoNode
{
	char*	pData;
	int		nLen;
};
//------------------------------------------------------------------------
class CGameSvrLink : public IFKAppNetworkLink, public IEventSink
{
private:
	typedef list< IDataReceiver* >		RECEIVER_LIST;
	typedef vector< RECEIVER_LIST >		MESSAGE_LIST;
private:
	MESSAGE_LIST						m_MsgList;		// ��Ϣ�б�
	CThreadRouteObject					m_Thread;
	HANDLE								m_hEventWakeDecode;
	list<stReceiverInfoNode>			m_NodeList;
	CCriticalSectionLock				m_Lock;
	BOOL								m_bThreadRun;
public:
	// ����/���� ������
	void					LockReceiver(BOOL bLock);
	// �����߳�״̬
	void					SetThreadRun(BOOL bRun);
	BOOL					Receiver();
	virtual void			OnEvent(DWORD dwEventID);

	// ���ӷ�����ʧ��
	// szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
	virtual void			OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode);

	// ���ӽ���
	virtual void			OnConnectOK(DWORD dwSocketID);

	// �յ�һ�����ݰ�
	// ע��	����������ݰ����߳���IThreadRoute����ͬһ�̣߳�
	//		����һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
	virtual BOOL			OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen);

	// ��ǰ���ӶϿ�
	// szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
	virtual void			OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode);

	// �������ݰ�
	virtual int				Send(LPCSTR buf, int nLen);

	// ȡ��IClientLinkָ��, ʹ�÷������ḻ
	virtual IClientLink *	GetClientLink(void);

	// ��Ҫ�������ȡ���ݵ�ģ��, ͨ���˺���ע���,�ſ��յ���Ϣ 
	BOOL					AddReceiver(IDataReceiver *pReceiver, int MsgCode);

	// �Ƴ�ģ�������
	BOOL					RemoveReceiver(IDataReceiver *pReceiver, int MsgCode);

	// ���ص�ǰ���ӵ�����һ��״̬
	int						GetType(void);
public:
	CGameSvrLink(void);
	virtual ~CGameSvrLink(void);
public:	
	IClientLink *	m_pClientLink;
};
//------------------------------------------------------------------------
extern CGameSvrLink g_GameSvrConnector;
//------------------------------------------------------------------------