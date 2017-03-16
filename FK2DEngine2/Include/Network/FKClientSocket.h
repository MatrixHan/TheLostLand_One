/**
*	created:		2013-5-10   1:26
*	filename: 		FKClientSocket
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define INVALID_SOCKET_ID			(-1)							// ��Ч��SOCKET ID
#define MAX_SOCKET_DATA_LEN			(16 * 1024)						// �û��������ߴ磬�����ٴ��ˣ��Ǻ�
#define MAX_SOCKET_RAW_DATA_LEN		(MAX_SOCKET_DATA_LEN + 256)		// �ײ�ͨѶ����󳤶ȣ����ಿ�����������ͷ��
#define MAX_SOCKET_BUF_LEN			MAX_SOCKET_RAW_DATA_LEN			// �ײ�ͨѶ����󳤶ȣ����ಿ�����������ͷ��
#define MAX_CLIENT_SOCKET			63								// IThreadRoute������ͬʱ����63��ͨѶ�����¼����������˳�����ܻ���Ҫ�����¼�������ͬʱ֧�ֵ����ӻ����
#define MAX_NAME_STRING_LEN			32								// �û��������롢��������������õ������
#define VERSION_SingleSocketProxy	1
#define VERSION_CLIENTLINK			2
//------------------------------------------------------------------------
// ���������������ض���
enum PROXY_TYPE
{
	proxyNone = 0, proxy4 = 4,proxy5 = 5
};
//------------------------------------------------------------------------
// ����������Ĳ��������趨
struct NET_PROXY_OPTION 
{
	NET_PROXY_OPTION ()
	{
		Close();
	}
	void Close()
	{
		memset(this,0,sizeof(NET_PROXY_OPTION )) ;
	}
	BOOL		bEnableProxy;								// �Ƿ�ʹ�ô��������
	PROXY_TYPE	socksType;									// ������������ͣ�Ĭ��Ϊ��֧�ִ��������
	char		szProxyServerName[MAX_NAME_STRING_LEN];		// �����������
	WORD		wProxyPort;									// ������������Ӷ˿ںţ���1080
	WORD		wReserved;
	BOOL		bCheckAuthority;							// ����������Ƿ���Ҫ�����֤
	char		szProxyUserName[MAX_NAME_STRING_LEN];		// �����Ҫ��֤����Ҫ�����������û���������
	char		szProxyPassword[MAX_NAME_STRING_LEN];
};
//------------------------------------------------------------------------
struct IClientSocket;
struct IClientSocketSink;
struct IEventSink;
struct IThreadRoute;
struct ITrace;
//------------------------------------------------------------------------
struct IClientSocket
{
	// ��ǰͨ����ʶ
	virtual DWORD GetSocketID() = NULL;

	// �������ӣ����ӽ�������յ�OnConnectOK()ʱ����ʾ���ӳɹ����յ�OnConnectError()ʱ��ʾ����ʧ��
	// ���أ��û��ɺ��Է��ؽ������������ʱҲ�᷵��FALSE
	virtual BOOL Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption = NULL) = NULL;

	// �رմ����ӣ��´ο����ٴε���Connect()��������
	// ����ص�IClientSocketSink::OnConnectClose()
	virtual BOOL CloseConnect() = NULL;

	/*
	 ���ܣ��������ݰ�
	 ���룺nLen���ܳ���MAX_SOCKET_RAW_DATA_LEN,buf����Ϊ��
	 ���أ����ʹ�����������FALSE
		   ������ͳɹ�������TRUE��WSA_IO_INCOMPLETE
				WSA_IO_INCOMPLETE��ʾ���ݷ��ͳɹ�����û���������ͳ�ȥ
				�����ļ����͵ȴ������ݴ���ʱ�����ã�Ӧ�ò���Ը����������ֵ�����ӳ��ٷ�
			    WSA_IO_INCOMPLETE��Overlapped sockets��WSASend()����WSAEWOULDBLOCK��Ϣʱ���ص�
				�����ϵ�ͬ��WSA_IO_INCOMPLETE,��ΪIClientSocket��ȴ�ʱ���ٴη���
	 ˵����1�������㷨Ϊ���������ͣ����ݷ���ͨʱ�Ž����ݷ��뷢�Ͷ��У�����WSA_IO_INCOMPLETE���ȴ��´η���
		   2�����socket����ʧ�ܣ�ϵͳ����OnConnectClose�з��س�����Ϣ����˲����ڷ��ʹ�������������
	*/
	virtual int Send(LPCSTR buf,int nLen) = NULL;

	// ���ܣ����յ�OnDataArrived()֪ͨʱ�����ܵ��ô˽ӿڽ�������
	// ���룺nBufLenΪbuf��С
	// ���أ�buf�к��г���ΪnLen�����ݣ��������TRUE����nLenΪ0����ʾ��ʱ�����ݷ���
	virtual BOOL Recv(int nMaxBufLen,/*out*/char* buf,/*out*/int& nLen) = NULL;

	// ��ѯ��ǰ�Ƿ�������״̬
	virtual BOOL IsConnected() = NULL;

	/* �л�����һ��״̬
	   �磺
		  Ӧ�ò�ĵ�¼̬������̬����������ʵ�֣���ʵ��IClientSocketSink
		  ����¼����Ժ󣬵��ô˺����ӵ�¼̬�л�������̬
	*/
	virtual void GotoNextState(IClientSocketSink* pSink) = NULL;

	// �������Ի�ȡ������һЩ�ڲ����ݣ��統ǰ���Ͷ��г��ȡ��շ��ٶȡ���ǰsocket�Ƿ������ȡ��ݲ�֧�ִ˹��ܡ�
	enum 
	{
		setMaxSendQueueCount = 1,		// ���ͻ��������󳤶����ã�Ĭ��Ϊ10000��dwParamΪ�µĳ��ȣ�pData���ص�ǰ����
		getMaxSendQueueCount = 2,		
		getSocketHandle = 3,			// ��ȡsocket������Ա�ֱ�Ӷ�socket���в���������setsockopt()
	};
	/*
		����Ϊ�������ܴ��Ӧ������һ������ķ��Ͷ��У�ioctrl(IClientSocket::setMaxSendQueueCount,1000000,NULL);
	*/
	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData) = NULL;

	// �ͷŶ���
	virtual void Release() = NULL;
};
/**************************************************************
	���ܣ�
		���������ӣ�֧�ֶ��߳�ͬ��֧��(����HWND)�͵��̸߳������ݴ���
 **************************************************************/
struct ILinkProxy
{
	virtual BOOL AddSocket(DWORD dwSocketID,IClientSocketSink *pSink,IClientSocket** ppClientSocket) = NULL;
	virtual BOOL RemoveSocket(IClientSocket* pClientSocket) = NULL;
	virtual void Release() = NULL;
};

//------------------------------------------------------------------------
/*
 * ˵����
 *			��ΪIClientSocketSink��IThreadRoute�б��ص�������:
 *			���ڷ������˳��򣬽�����IClientSocketSink�����ڷ������߼���ʵ�֣���ʱ������
 *			����������ͬһ�̣߳����ؿ����߳�ͬ������
 *			�����ڿͻ��˳���ͨѶ�����������ͬһ�̣߳���Ҫ�����߳�ͬ������
 *
 */
struct IClientSocketSink
{
	// ���ӷ�����ʧ��
	// szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
	virtual void OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;

	// ���ӽ���
	virtual void OnConnectOK(DWORD dwSocketID) = NULL;
	
	// �����ݿ��Խ���
	// ˵������ʱ�ײ����m_pClientSocket->Recv()��ϵͳsocket���������
	// ע��	����������ݰ����߳���IThreadRoute����ͬһ�̣߳�
	//		����һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
	virtual void OnDataArrived(DWORD dwSocketID) = NULL;

	/* ��ǰ���ӶϿ�
	   szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
	   ע�⣺
			1��Ӧ�ò����ڶ�����¼��������Ժ�ִ��IClientSocket::CloseConnect()����������Դ
	  		   ����������Ϊ��Ϊ��֤Ӧ�ò������ӶϿ�ʱ�ܽ��յ��������ݣ�IClientSocket��û������������Դ
			   ������HWNDͬ�����߳�ģʽ��������Ҫ����ΪPostMessage()���͵���Ϣ������ʱ��ǳ���
			2��CreateSingleSocketProxy()������IClientSocket�Ѿ������û����CloseConnect()����
	*/
	virtual void OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;
};
//------------------------------------------------------------------------
extern "C" BOOL CreateLinkProxyWnd(IThreadRoute *pThreadRoute,ITrace* pTrace,BOOL bSyncByWnd,BOOL bInSameThread,/*out*/ILinkProxy** ppLinkProxy);
// ���룺
//		dwSocketID	:	�൱��SetTimer()�е�dwTimerID���û��ô�ID��ʶ��ǰ����
//						�����������û���һ��IClientSocketSink���������ӣ�һ��ȡ�û������ӵ���������
//						����ͨ��IClientSocket::GetSocketID()��ȡ����ʱ��dwSocketID
//		bInSameThread : ���̺߳�ͨѶ�߳��Ƿ���ͬһ�̣߳�������Ӧ�����������ͬһ�̣߳��Ӷ�����ʹ���û�����
extern "C" BOOL CreateClientSocket(DWORD dwSocketID,IClientSocketSink *pSink,IThreadRoute *pThreadRoute, ITrace* pTrace,BOOL bInSameThread,/*out*/IClientSocket** ppClientSocket);
// ����һ�����ӣ���һ���̹߳���һ�����ӣ��ʺ������Ӻ��٣�����Ҫ�󲻸ߵ���������д��ڵĿͻ��˳���ͨѶ����
extern "C" BOOL CreateSingleSocketProxy(DWORD dwSocketID,IClientSocketSink* pSink,ITrace* pTrace,/*out*/IClientSocket** ppClientSocket);
//------------------------------------------------------------------------
class CSingleSocketProxyHelper
{
public:
	CSingleSocketProxyHelper()
	{
		m_pClientSocket = NULL;
	}
	~CSingleSocketProxyHelper()
	{
		Close();
	}
	void Close()
	{
		if(m_pClientSocket)
		{
			m_pClientSocket->Release();
			m_pClientSocket = NULL;
		}
	}
	BOOL Create(DWORD dwSocketID,IClientSocketSink* pSink,ITrace* pTrace)
	{
		Close();

		if(! MyCreateSingleSocketProxy(VERSION_SingleSocketProxy,dwSocketID,pSink,pTrace,&m_pClientSocket))
		{
			return FALSE;
		}
		return m_pClientSocket != NULL;
	}
public:
	BOOL MyCreateSingleSocketProxy(DWORD dwVersion,DWORD dwSocketID,IClientSocketSink* pSink,ITrace* pTrace,/*ou*/IClientSocket** ppClientSocket);
public:
	IClientSocket* m_pClientSocket;
};
//------------------------------------------------------------------------
// ������͵�¼�����ӹ���

/* �����¼�������
 ˵����
		1�������ӿ��Ժ������е�dwSocketIDֵ
		2������е�¼���̣��������״̬ģʽ��
			����¼̬������̬��ʵ��IClientLinkSink�ӿڣ�
			����¼����Ժ����IClientLink::GotoNextState(IClientLinkSink* pSink)ʵ��״̬�л�
 */
struct IClientLinkSink
{
	// ���ӷ�����ʧ��
	// szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
	virtual void OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;

	// ���ӽ���
	virtual void OnConnectOK(DWORD dwSocketID) = NULL;
	
	// �յ�һ�����ݰ�
	// ע��	����������ݰ����߳���IThreadRoute����ͬһ�̣߳�
	//		����һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
	virtual BOOL OnRecv(DWORD dwSocketID,LPCSTR buf,int nLen) = NULL;

	// ��ǰ���ӶϿ�
	// szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
	virtual void OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;
};
//------------------------------------------------------------------------
// ֧�ֵ�¼���̡����ͼ��ܹ��������
struct IClientLink
{
	// ��ǰͨ����ʶ
	// �����ӿ��Ժ���dwSocketIDֵ
	virtual DWORD GetSocketID() = NULL;

	/*
		�������Ӳ���¼������

		������
			szServerAddress��wServerPort:
				��������ַ�Ͷ˿�
			pProxyOption:
				������������������ΪNULL����ʾ��֧�ִ��������

		���أ�
			�û��ɺ��Է��ؽ������������ʱҲ�᷵��FALSE
			�ɹ���TRUE
			ʧ�ܣ�FALSE
				�������ʧ�ܣ�IClientLinkSink::OnConnectError()�����ص�
				������ӳɹ������ڵȴ���¼���ʱ���ӶϿ���IClientLinkSink::OnConnectClose()��������
	*/
	virtual BOOL Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption = NULL) = NULL;

	/* �л�����һ��״̬
	   �磺
		  Ӧ�ò�ĵ�¼̬������̬����������ʵ�֣���ʵ��IClientLinkSink
		  ����¼����Ժ󣬵��ô˺����ӵ�¼̬�л�������̬
	*/
	virtual void GotoNextState(IClientLinkSink* pSink) = NULL;

	// �رմ����ӣ��´ο����ٴε���Connect()�������ӣ�IClientLinkSink::OnLoginResult()���ᱻ����
	// ����ص�IClientSocketSink::OnConnectClose()
	virtual BOOL CloseConnect() = NULL;

	virtual BOOL GetCallList( void ** pList , int & nLen , int nVer ) = NULL;
	
	/*
	 ���ܣ��������ݰ�
	 ���룺nLen���ܳ���16 x 1024,buf����Ϊ��
	 ���أ����ʹ�����������FALSE
		   ������ͳɹ�������TRUE��WSA_IO_INCOMPLETE
				WSA_IO_INCOMPLETE��ʾ���ݷ��ͳɹ�����û���������ͳ�ȥ
				�����ļ����͵ȴ������ݴ���ʱ�����ã�Ӧ�ò���Ը����������ֵ�����ӳ��ٷ�
			    WSA_IO_INCOMPLETE��Overlapped sockets��WSASend()����WSAEWOULDBLOCK��Ϣʱ���ص�
				�����ϵ�ͬ��WSA_IO_INCOMPLETE,��ΪIClientSocket��ȴ�ʱ���ٴη���
	 ˵����1�������㷨Ϊ���������ͣ����ݷ���ͨʱ�Ž����ݷ��뷢�Ͷ��У�����WSA_IO_INCOMPLETE���ȴ��´η���
		   2�����socket����ʧ�ܣ�ϵͳ����OnConnectClose�з��س�����Ϣ����˲����ڷ��ʹ�������������
	*/
	virtual int Send(LPCSTR buf,int nLen) = NULL;

	// ��ѯ��ǰ�Ƿ�������״̬
	virtual BOOL IsConnected() = NULL;

	// ����һЩ����
	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData) = NULL;

	// �ͷŶ���
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
/*
  ���ܣ�
		����һ��֧�ַ�������ܡ���¼���ӹ������
  ˵�������ù�ϵ����

			    IClientLinkSink				 IClientSocketSink
		Ӧ�ò� <--------------- IClientLink <------------------- IClientSocket
		       ---------------->			------------------->
			     IClientLink				 IClientSocket
  ������
	pSink			: ͨѶ�¼�������

  ��ѡ����:
    pOption			: ����δ�������Զ����IKeyStream�ӿڡ���������ѡ���
    pClientSocket	: �����ǵ��߳�ģʽ����߳�ģʽ��IClientSocket
					  ��pClientSocket == NULL,��ʾȡ��CreateClientLink�ڲ�ͨ��
						CreateSingleSocketProxy()����һ������HWNDͬ����IClientSocket
	pTrace			: ͨѶ��ĵ�����Ϣ��ʾ�ӿ�

�����أ�
		���ش�������IClientLink����
*/
//------------------------------------------------------------------------
extern "C" IClientLink* CreateClientLink(DWORD dwSocketID,IClientLinkSink* pSink,ITrace* pTrace = NULL,IClientSocket* pClientSocket = NULL,void* pOption = NULL);
//------------------------------------------------------------------------
class CClientLinkHelper
{
public:
	CClientLinkHelper()
	{
		m_pClientLink = NULL;
	}
	~CClientLinkHelper()
	{
		Close();
	}
	void Close()
	{
		if(m_pClientLink)
		{
			m_pClientLink->Release();
			m_pClientLink = NULL;
		}
	}
	BOOL Create(DWORD dwSocketID,IClientLinkSink* pSink,ITrace* pTrace = NULL,IClientSocket* pClientSocket = NULL,void* pOption = NULL)
	{
		if(m_pClientLink)
		{
			m_pClientLink->Release();
			m_pClientLink = NULL;
		}
		m_pClientLink = MyCreateClientLink(VERSION_CLIENTLINK,dwSocketID,pSink,pTrace,pClientSocket,pOption);
		if(m_pClientLink == NULL)
		{
			return false;
		}
		return TRUE;
	}
public:
	IClientLink* MyCreateClientLink(DWORD dwVersion,DWORD dwSocketID,IClientLinkSink* pSink,ITrace* pTrace,IClientSocket* pClientSocket,void* pOption);
public:
	IClientLink* m_pClientLink;
};
//------------------------------------------------------------------------