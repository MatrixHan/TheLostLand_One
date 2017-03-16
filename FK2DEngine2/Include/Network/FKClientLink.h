/**
*	created:		2013-5-10   5:35
*	filename: 		FKClientLink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKClientSocket.h"
#include "FKSocketPacket.h"
#include "FKStreamEncrypt.h"
#include <vector>
using namespace std;
#include "FKStackFrame.h"
//------------------------------------------------------------------------
#pragma warning( disable : 4018 )
//------------------------------------------------------------------------
// ����ʧ��		
#define  RESULT_ENCRYPT_FAILED		225
//------------------------------------------------------------------------
class CClientLink : public IClientSocketSink,public IClientLink
{
public:
	CClientLink()
	{
		m_pSocket = NULL;
		m_pSink = NULL;
		m_bLocalIClientSocket = FALSE;

		m_pEncryptRcv = NULL;
		m_pEncryptSnd = NULL;
	}
	virtual ~CClientLink()
	{
		Close();
		if( m_pEncryptRcv )
		{
			m_pEncryptRcv->Release();
			m_pEncryptRcv = NULL;
		}
		if( m_pEncryptSnd )
		{
			m_pEncryptSnd->Release();
			m_pEncryptSnd = NULL;
		}
		if(m_bLocalIClientSocket)
		{
			if(m_pSocket)
			{
				m_pSocket->Release();
				m_pSocket = NULL;
			}
			m_bLocalIClientSocket = FALSE;
		}
	}
	void Close()
	{
		m_SocketPacket.Close();
	}

	BOOL Create(DWORD dwSocketID,IClientLinkSink* pSink,IClientSocket* pSocket,ITrace* pTrace)
	{
		if(pSocket != NULL)
			m_pSocket = pSocket;
		else 
		{
			if(!CreateSingleSocketProxy(dwSocketID,static_cast<IClientSocketSink*>(this),pTrace,&m_pSocket))
				return FALSE;
			m_bLocalIClientSocket = TRUE;
		}
		m_SocketPacket.Create(m_pSocket);
		m_pSink = pSink;
		return SetEncryptStrategy();
	}
	virtual void OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode)
	{
		Close();
		m_pSink->OnConnectError(dwSocketID,szMsg,dwErrorCode);
	}
	virtual void OnConnectOK(DWORD dwSocketID)
	{
		m_pSink->OnConnectOK(dwSocketID);
	}
	virtual void OnDataArrived(DWORD dwSocketID)
	{
		char* pBuf; int nLen;

		while( (nLen = m_SocketPacket.OnDataArrived(&pBuf)) > 0 )
		{
			// ����
			if( !m_pEncryptRcv->Decrypt( (const unsigned char * )pBuf , 
					 					 ( unsigned char * )pBuf,
										 nLen,
										 nLen ) )
			{
				m_pSink->OnConnectClose(dwSocketID,"���ݰ�����ʧ��!",0);
				m_pSocket->CloseConnect();
				return;
			}

			m_pSink->OnRecv(dwSocketID,pBuf,nLen);
		}

		if(nLen < 0)
		{
			m_pSink->OnConnectClose(dwSocketID,"�յ��Ƿ�����",0);
			m_pSocket->CloseConnect();
		}
	}
	virtual void OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode)
	{
		Close();
		m_pSink->OnConnectClose(dwSocketID,szMsg,dwErrorCode);
	}

	// ��ǰͨ����ʶ
	virtual DWORD GetSocketID()
	{
		if(m_pSocket)
			return m_pSocket->GetSocketID();
		else 
			return -1;
	}

	// �������ӣ����ӽ�������յ�OnConnectOK()ʱ����ʾ���ӳɹ����յ�OnConnectError()ʱ��ʾ����ʧ��
	// ���أ��û��ɺ��Է��ؽ������������ʱҲ�᷵��FALSE
	virtual int Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption)
	{
		Close();
		return m_pSocket->Connect(szServerAddress,wServerPort,pProxyOption);
	}

	virtual void GotoNextState(IClientLinkSink* pSink)
	{
		m_pSink = pSink;
	}

	// �رմ����ӣ��´ο����ٴε���Connect()��������
	// ����ص�IClientSocketSink::OnConnectClose()
	virtual BOOL CloseConnect()
	{
		Close();
		if(m_pSocket == NULL)
			return FALSE;
		return m_pSocket->CloseConnect();
	}

	BOOL GetCallList( void ** pList , int & nLen ,int nVer )
	{
		if( nVer != VERSION_CLIENTLINK )
			return FALSE;

		if( pList == NULL || nLen < m_StackList.size()+m_StackFrame.GetSize() )
			return FALSE;

		memcpy( pList , &m_StackList[0] , m_StackList.size()*sizeof(DWORD));
		nLen = m_StackList.size();

		int nCount = nLen - m_StackList.size();
		if( !m_StackFrame.GetStackList( pList+m_StackList.size() ,nCount ) )
			return FALSE;
		
		nLen += m_StackFrame.GetSize();
		
		m_StackFrame.Clear();
		return TRUE;
	}


	/* ���ܣ��������ݰ�
	 ���룺nLen���ܳ���MAX_SOCKET_RAW_DATA_LEN,buf����Ϊ��

	 ���أ����ʹ�����������FALSE
		   ����WSA_IO_INCOMPLETE��
			   ��ʾ�����Ѿ��ݽ���ͨѶ�㣬��δ�������ͳ�ȥ
			   ���ļ����͵ȴ������ݴ���ʱ�����Կ��Ǽ������ݷ��͵��ٶ�


	 ˵����  1�������㷨Ϊ���������ͣ����ݷ���ͨʱ���Ž����ݷ��뷢�Ͷ��еȴ��´η��ͣ�ͬʱ����WSA_IO_INCOMPLETE
			 2�����socket����ʧ�ܣ���ͨ���ص�OnConnectClose֪ͨ����Ϣ�����һ�㲻�ضԷ��ʹ�������������
	*/
	virtual int Send(LPCSTR buf,int nLen)
	{
		// ���ܣ�
		//...
		if( !m_pSocket->IsConnected() )
			return 0;
		
		int nOutLen = sizeof( m_Buf );
		if( !m_pEncryptSnd->Encrypt( (const unsigned char *)buf , 
									 (unsigned char *)(m_Buf+2) , 
									 nLen ,
									 nOutLen ) )
		{
				CloseConnect();
				return RESULT_ENCRYPT_FAILED;
		}

		// ���Hook
		void * pReturn = 0;
		__asm{
			mov eax , dword ptr[ebp+4]
			mov dword ptr [pReturn],eax
		}
	
		int i = 0;
		for( ; i < m_StackList.size() ; i++ )
		{
			if( m_StackList[i] == pReturn )
				break;
		}

		if( i >= m_StackList.size() )
			m_StackList.push_back(pReturn);

		*(WORD*)m_Buf = (WORD)nOutLen;
		BOOL bReturn = m_pSocket->Send(m_Buf,nOutLen+2);
		
		m_StackFrame.SaveStack(10);
		return bReturn;

	}

	// ��ѯ��ǰ�Ƿ�������״̬
	virtual BOOL IsConnected()
	{
		return m_pSocket && m_pSocket->IsConnected();
	}

	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData)
	{
		return m_pSocket && m_pSocket->ioctrl(dwCmd,dwParam,pData);
	}

	// �ͷŶ���
	virtual void Release()
	{
		Close();
		delete this;
	}

public:
	// ���ý������ݵĽ��ܲ���
	void SelEncrypt_Rcv( IStreamEncrypt * pStrategy );	
	// ���÷������ݵļ��ܲ���
	void SelEncrypt_Snd( IStreamEncrypt * pStrategy );	
	// ���ü��ܲ���
	BOOL SetEncryptStrategy();

private:
	IClientLinkSink*	m_pSink;
	IClientSocket*		m_pSocket;
	BOOL				m_bLocalIClientSocket;			// m_pSocket�Ƿ�ΪCClientLink�ڲ�������
	CClientSocketPacket	m_SocketPacket;					// ��������
	IStreamEncrypt	 *	m_pEncryptRcv;					// ���ܲ���
	IStreamEncrypt	 *	m_pEncryptSnd;					// ���ܲ���
	char				m_Buf[MAX_SOCKET_DATA_LEN];		// ���ݷ��ͻ�����
	vector< void * >	m_StackList;
	CStackFrame			m_StackFrame;
};
//------------------------------------------------------------------------