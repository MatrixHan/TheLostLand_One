/**
*	created:		2013-5-10   6:39
*	filename: 		FKSingleSocketProxy
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKCThreadRoute.h"
#include "FKLinkProxyWnd.h"
#include <winsock2.h>
//------------------------------------------------------------------------
class CSingleSocketProxy : public IClientSocket
{
public:
	void Close();
	CSingleSocketProxy();
	~CSingleSocketProxy();

	BOOL Create(DWORD dwSocketID,IClientSocketSink *pSink);

	// ��ǰͨ����ʶ
	virtual DWORD GetSocketID()
	{
		return 0;
	}

	// �������ӣ����ӽ�������յ�OnConnectOK()ʱ����ʾ���ӳɹ����յ�OnConnectError()ʱ��ʾ����ʧ��
	// ���أ��û��ɺ��Է��ؽ��
	virtual BOOL Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption = NULL)
	{
		return m_pClientSocket && m_pClientSocket->Connect(szServerAddress,wServerPort,pProxyOption);
	}

	// �رմ����ӣ��´ο����ٴε���Connect()��������
	// ����ص�IClientSocketSink::OnConnectClose()
	virtual BOOL CloseConnect()
	{
		return m_pClientSocket && m_pClientSocket->CloseConnect();
	}

	// ���������ͣ����ݷ���ͨʱ�Ž����ݷ��뷢�Ͷ��еȴ��´η���
	// ���룺nLen���ܳ���MAX_SOCKET_RAW_DATA_LEN,buf����Ϊ��
	// ���أ����socket����ʧ�ܣ�ϵͳ����OnSocketError()�з��س�����Ϣ����˲����ڷ��ʹ����ؼ�����
	//		 ������ʹ�����������FALSE
	//		 ������ͳɹ����������æ������WSA_IO_INCOMPLETE���������ֵ���ļ����͵ȴ������ݴ���ʱ������
	virtual int Send(LPCSTR buf,int nLen)
	{
		if(m_pClientSocket == NULL)
			return FALSE;
		int nReturn = m_pClientSocket->Send(buf,nLen);

		// �����¼�
		CLinkProxyWnd::LINK_NOTE *pNode = new CLinkProxyWnd::LINK_NOTE(m_pClientSocket->GetSocketID(),NULL,0,0);
		if(pNode)
			::PostMessage(m_LinkProxyWnd.m_hWnd,WM_LINK_MSG,CLinkProxyWnd::linkDataSending,(LPARAM)pNode);

		return nReturn;
	}

	// ���յ�OnDataArrived()֪ͨʱ�����ܵ��ô˽ӿڽ�������
	// ���룺nBufLenΪbuf��С
	// ���أ�buf�к��г���ΪnLen������
	virtual BOOL Recv(int nBufLen,/*out*/char* buf,/*out*/int& nLen)
	{
		return m_pClientSocket && m_pClientSocket->Recv(nBufLen,buf,nLen);
	}

	// ��ѯ��ǰ�Ƿ�������״̬
	virtual BOOL IsConnected()
	{
		return m_pClientSocket && m_pClientSocket->IsConnected();
	}

	/* �л�����һ��״̬
	   �磺
		  Ӧ�ò�ĵ�¼̬������̬����������ʵ�֣���ʵ��IClientSocketSink
		  ����¼����Ժ󣬵��ô˺����ӵ�¼̬�л�������̬
	*/
	virtual void GotoNextState(IClientSocketSink* pSink)
	{
		m_pSink = pSink;
	}

	// �������Ի�ȡһЩ�ڲ����ݣ��統ǰ���Ͷ��г��ȡ��շ��ٶȡ���ǰsocket�Ƿ������ȡ��ݲ�֧�ִ˹��ܡ�
	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData)
	{
		return m_pClientSocket && m_pClientSocket->ioctrl(dwCmd,dwParam,pData);
	}

	// �ͷŶ���
	virtual void Release()
	{
		Close();
		delete this;
	}
private:
	CThreadRoute		m_ThreadRoute;
	CLinkProxyWnd		m_LinkProxyWnd;
	IClientSocketSink*	m_pSink;
	IClientSocket*		m_pClientSocket;
};
//------------------------------------------------------------------------