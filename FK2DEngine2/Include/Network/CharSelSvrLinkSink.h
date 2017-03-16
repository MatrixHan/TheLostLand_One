/**
*	created:		2013-5-10   8:21
*	filename: 		CharSelSvrLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IFKAppNetworkLink.h"
//------------------------------------------------------------------------
class CCharSelSvrLink : public IFKAppNetworkLink
{
public:
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

	// ���ص�ǰ���ӵ�����һ��״̬
	int						GetType(void);
public:
	CCharSelSvrLink(void);
public:	
	IClientLink *	m_pClientLink;
};
//------------------------------------------------------------------------
extern CCharSelSvrLink g_CharSelSvrConnector;
//------------------------------------------------------------------------