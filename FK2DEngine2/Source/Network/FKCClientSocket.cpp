/**
*	created:		2013-5-10   3:05
*	filename: 		FKCClientSocket
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/Network/FKCClientSocket.h"
#include "../../Include/FKITrace.h"
#include "../../Include/IFKFileSystem.h"
#include "../../Include/ResMgr/ResMgrCommon.h"
//------------------------------------------------------------------------
#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
//------------------------------------------------------------------------
// ����0x100��Ϊ��Ч��Ϣ��������WSAGetLastError()������Ϣ����
#define MSG_ERROR_MSG			0x100	// ��CNetLink::m_szLastInfo��������Ĵ�����Ϣ
#define TIMER_CHECK_SEND_QUEUE	100		// ��ʱ����Ƿ��ٴη������ݰ���
//------------------------------------------------------------------------
/*										
	�ݲ���ʵ��

	TIMER_CHECK_SEND_QUEUE������������⣺
		FD_WRITE����ͷ��Ͷ����Ƿ�Ϊ���������źſ��ܲ�ͳһ�����·��Ͷ������а���
		FD_WRITE�¼�����һ�Σ���Ҳ�����ˣ������Ͷ��в�ͣ�����ӡ����������ϲ�����
		����һ��������ͨѶ�㶨ʱ����һЩ�¼�
	
*/
//------------------------------------------------------------------------
int CClientSocket::m_nWSAStartupRefCount = 0;
//------------------------------------------------------------------------
CClientSocket::CClientSocket()
{
	m_ConnectState = stateClosed;

	m_dwCommEventID = INVALID_EVENT_ID;
	m_dwTimerEventID = INVALID_EVENT_ID;
	m_dwConnectEventID = INVALID_EVENT_ID;
	m_hRefCommEvent = NULL;

	m_SocketClient = INVALID_SOCKET;
	m_szServerAddress[0] = 0;
	m_wServerPort = -1;

	m_dwSocketID = INVALID_SOCKET_ID;
	m_pThreadRoute = NULL;
	m_pSink = NULL;
	m_pTrace = NULL;

	m_nMaxSendQueueCount = 10000;

	m_hOverlapedEvent = (WSAEVENT)CreateEvent(NULL,FALSE,FALSE,NULL);
}
//------------------------------------------------------------------------
CClientSocket::~CClientSocket()
{
	if(m_hOverlapedEvent)
		CloseHandle(m_hOverlapedEvent);
}
//------------------------------------------------------------------------
extern "C" BOOL CreateClientSocket(DWORD dwSocketID,IClientSocketSink *pSink,IThreadRoute *pThreadRoute, ITrace* pTrace,BOOL bInSameThread,/*out*/IClientSocket** ppClientSocket)
{
	if(ppClientSocket == NULL)
		return FALSE;
	CClientSocket *pSocket = new CClientSocket();
	if(pSocket == NULL)
		return FALSE;
	if(!pSocket->Create(dwSocketID,pSink,pThreadRoute,pTrace,bInSameThread))
	{
		DebugMsgLn(L"Error : CLinkProxyWnd::AddSocket() : Can not create CClientSocket");
		delete pSocket;
		return FALSE;
	}
	*ppClientSocket = static_cast<IClientSocket*>(pSocket);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CClientSocket::StartupSocket()
{
	m_nWSAStartupRefCount++;
	if(m_nWSAStartupRefCount > 1)
		return TRUE;
	WSADATA WSAData;
	int  retv = WSAStartup(0x0202,&WSAData);
	if(retv == SOCKET_ERROR)
	{
		TraceLn("��ʼ�� Winsock2 ʧ��!");
		m_nWSAStartupRefCount--;
		return FALSE;
	}
	if( (WSAData.wHighVersion & 0xFF00) != 0x0200)
	{
		TraceLn("Windows Socket �汾̫��!");
		m_nWSAStartupRefCount--;
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void DumpData(LPCSTR szDataName,LPCSTR buf, int nLen)
{
	if(buf == NULL)
		return;
	wchar_t str[4096] = L"",temp[1024] = L"";
	for(int i = 0; i < nLen; i++)
	{
		wsprintf(temp,L"%02X ", (BYTE)buf[i]);
		wcscat(str,temp);
	}
	DebugMsgLn(str);
}
//------------------------------------------------------------------------
// �رմ����ӣ��´ο����ٴε���OpenLink()��������
BOOL CClientSocket::CloseConnect()
{
	ClearSocketData(); 
	return TRUE;
}
//------------------------------------------------------------------------
// bInSameThread�����Ż������Ѿ�֪����ͬһ�̻߳���ʱ������ʹ��ͬ����
BOOL CClientSocket::Create(DWORD dwSocketID,IClientSocketSink *pSink,IThreadRoute *pThreadRoute, ITrace* pTrace,BOOL bInSameThread)
{
	Assert(pSink != NULL);
	Assert(pThreadRoute != NULL);

	if(!StartupSocket())
		return FALSE;

	m_dwSocketID = dwSocketID;  
	m_pThreadRoute = pThreadRoute;
	m_pSink = pSink;

	m_LockClientSocketContext.SetNoneLock(bInSameThread);

	Assert(m_pThreadRoute->AddEvent(static_cast<IEventSink*>(this),m_dwCommEventID));
	m_hRefCommEvent = m_pThreadRoute->GetEventHandle(m_dwCommEventID);
	Assert(m_hRefCommEvent != NULL);
	return TRUE;
}
//------------------------------------------------------------------------
// ���Create��������Դ
void CClientSocket::Close()
{
	ClearSocketData();
	if(m_pThreadRoute && m_dwCommEventID != INVALID_EVENT_ID)
	{
		m_pThreadRoute->RemoveEvent(m_dwCommEventID);
		m_dwCommEventID = INVALID_EVENT_ID;
		m_pThreadRoute = NULL;
	}
	m_nWSAStartupRefCount--;
}
//------------------------------------------------------------------------
// ���ܣ����ӷ���������CloseConnect���
// �������ӣ����ӽ�������յ�OnConnectOK()ʱ����ʾ���ӳɹ����յ�OnConnectError()ʱ��ʾ����ʧ��
// ���أ��û��ɺ��Է��ؽ��
BOOL CClientSocket::Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption)
{
	ClearSocketData();
	Assert(szServerAddress != NULL);
	lstrcpynA(m_szServerAddress,szServerAddress,MAX_PATH);
	m_wServerPort = wServerPort;
	if(pProxyOption)
		memcpy(&m_ProxyOption,pProxyOption,sizeof(NET_PROXY_OPTION));

	// ��Ϊ���������������ٶȺ��������Խ����ӹ��̸���ͨѶ�߳���ִ��
	m_ConnectState = stateReqConnect;
	SetEvent(m_hRefCommEvent);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CClientSocket::GetIP(char *szName, in_addr *pAddr)
{
	Assert(szName != NULL);
	Assert(pAddr != NULL);
	int nPointCount = 0;int len = strlen(szName);
	int i = 0;
	for( ; i < len; i++)
	{
		if(szName[i] == '.')
			nPointCount++;
		else if(!isdigit(szName[i]))
			break;
	}
	if(nPointCount < 3 || i < len)	// not IP Address
	{
		struct hostent *hp = NULL;
		hp = gethostbyname(szName);
		if (hp == NULL ) 
		{
			char szMsg[1024];
			wsprintfA(szMsg,"������������:%s����",szName);
			ShowConnectError(szMsg,0);
			return FALSE;
		}
		memcpy(pAddr,hp->h_addr,hp->h_length);
	}
	else
	{
		int ip[128];BYTE *p = (BYTE*)pAddr;
		sscanf(szName,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);
		for( i = 0; i < 4; i++)
			p[i] = (BYTE)ip[i];
	}
	return TRUE;
}
//------------------------------------------------------------------------
// ����ʱ��������
void CClientSocket::ShowConnectError(char *szErrorMsg, int nErrorCode)
{
	if(m_pSink)
		m_pSink->OnConnectError(m_dwSocketID,szErrorMsg,nErrorCode);

	ClearSocketData();
}
//------------------------------------------------------------------------
//�����ӳɹ��Ժ�������
void CClientSocket::ShowSocketError(char *szErrorMsg, int nErrorCode)
{
	if(!IsClosed())
	{ // �����ǰ����ʵ�����Ѿ��Ͽ���������Ϣ�����ٴ��ݳ�ȥ
		if(m_pSink)
			m_pSink->OnConnectClose(m_dwSocketID,szErrorMsg,nErrorCode);
		// ��Ϊ���ӶϿ�ʱ�������ݻ���Ч����Ӧ�ò�һ�������ݵĻ��ᣬ�����ݲ��������
	}
}
//------------------------------------------------------------------------
void CClientSocket::OnEvent(DWORD dwEventID)
{
	if(dwEventID != m_dwCommEventID)
		return;

	if(m_ConnectState == stateReqConnect)
	{ // ������������
		m_ConnectState = stateConnecting;
		DoConnect(m_szServerAddress,m_wServerPort,&m_ProxyOption);
		return;
	}

	WSANETWORKEVENTS	events;
	int retv = WSAEnumNetworkEvents(m_SocketClient , m_hRefCommEvent,&events);
	if(retv == SOCKET_ERROR)
	{
		ShowSocketError("EnumNetworkEvent failed",WSAGetLastError());
		return;
	}
	if(events.lNetworkEvents == 0)
		return;
	long lEvents = events.lNetworkEvents;
	if(lEvents & FD_CONNECT)
	{
		int nErrorCode = events.iErrorCode[FD_CONNECT_BIT];
		if(nErrorCode == 0)
		{
			m_ConnectState = stateConnected;
			m_pSink->OnConnectOK(m_dwSocketID);
		}
		else
		{
			ShowConnectError("��������ַ����ȷ��������������У����Ժ�����",nErrorCode);
			return;
		}
	}
	if(!IsConnected())
	{
#ifdef _DEBUG
		wchar_t buf[1024];
		wsprintf(buf,L"Warn : CClientSocket::OnEvent() : socket[%d] is not connected now\n",m_dwSocketID);
		OutputDebugString(buf);
#endif
		return;
	}
	if(lEvents & FD_READ)
	{
		int nErrorCode = events.iErrorCode[FD_READ_BIT];
		if(nErrorCode != 0)
			ShowConnectError("�������ݳ���",nErrorCode);
		else
			m_pSink->OnDataArrived(m_dwSocketID);
	}
	if(lEvents & FD_WRITE)
	{
		int nErrorCode = events.iErrorCode[FD_WRITE_BIT];
		if(nErrorCode != 0)
			ShowConnectError("�������ݳ���",nErrorCode);
		else
			OnFD_WRITE();

	}
	if(lEvents & FD_CLOSE)
	{
		ShowSocketError("�������ʧȥ��ϵ",0);
	}
}
//------------------------------------------------------------------------
BOOL CClientSocket::Recv(int nBufLen,/*out*/char* buf,/*out*/int& nLen)
{
	int retv = recv(m_SocketClient,buf,nBufLen,0);
	if(retv == 0)
	{
		ShowSocketError("��������ʱ�Է��ر�����",0);
		return FALSE;
	}
	else if(retv == SOCKET_ERROR)
	{
		if(WSAGetLastError() == WSAEWOULDBLOCK)	// ��ǰ���ղ�������
		{
			nLen = 0;							// û�н��յ�����
			return TRUE;
		}
		ShowSocketError("�������ݳ���",WSAGetLastError());
		return FALSE;
	}
	nLen = retv;
	return TRUE;
}
//------------------------------------------------------------------------
DWORD CClientSocket::OnTimer(DWORD dwTimerID,DWORD dwParam)
{
	OnFD_WRITE();
	return  0;
}
//------------------------------------------------------------------------
// ���������ͣ����ݷ���ͨʱ�Ž����ݷ��뷢�Ͷ��еȴ��´η���
int CClientSocket::Send(LPCSTR buf,int nLen)
{
	m_LockClientSocketContext.Lock();
	if(!IsConnected())
	{
		m_LockClientSocketContext.Unlock();
		return FALSE;
	}
	Assert(buf != NULL);
	Assert(nLen > 0 && nLen <= MAX_SOCKET_RAW_DATA_LEN);

	// ������в�Ϊ�գ������������ͣ����ǷŶ����еȴ�FD_WRITE�¼�����ʱ���ܷ���
	if(m_SendQueue.size() > m_nMaxSendQueueCount)	// ����������ۻ���̫�࣬�رմ�����
	{
		m_LockClientSocketContext.Unlock();
		ShowSocketError("���ݰ��ۻ����࣬�رյ�ǰ����",0);
		return FALSE;
	}
	WSABUF wsabuf;
	wsabuf.len = nLen;

	if(m_SendQueue.empty())
	{
		wsabuf.buf = (char*)buf;
		DWORD dwByteSent = 0;
		DWORD dwReturn =  DoSend(wsabuf,dwByteSent);
		if(dwReturn == 0)
		{
			m_LockClientSocketContext.Unlock();
			return TRUE;
		}
		else if(dwReturn == WSA_IO_INCOMPLETE)
		{
			nLen = nLen - dwByteSent;
			if(nLen <= 0)
			{
				m_LockClientSocketContext.Unlock();
				return TRUE;
			}
			wsabuf.buf = MyAllocMemory(nLen);
			if(wsabuf.buf == NULL)
			{
				m_LockClientSocketContext.Unlock();
				ShowSocketError("��������ʱ�ڴ治��",0);
				return FALSE;
			}
			wsabuf.len = nLen;
			memcpy(wsabuf.buf,buf + dwByteSent,nLen);
			m_SendQueue.push_back(wsabuf);
			m_LockClientSocketContext.Unlock();
			return WSA_IO_INCOMPLETE;
		}
		if(dwReturn == SOCKET_ERROR)
		{
			m_LockClientSocketContext.Unlock();
			ShowSocketError("��������ʧ��",WSAGetLastError());
			return FALSE;
		}
	}
	else
	{
		wsabuf.buf = MyAllocMemory(nLen);
		if(wsabuf.buf == NULL)
		{
			m_LockClientSocketContext.Unlock();
			ShowSocketError("��������ʱ�ڴ治��",0);
			return FALSE;
		}
		memcpy(wsabuf.buf,buf,nLen);
		m_SendQueue.push_back(wsabuf);
		m_LockClientSocketContext.Unlock();
		return WSA_IO_INCOMPLETE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// ����:
//		WSA_IO_INCOMPLETE	: ��ǰ�����ٷ��ͣ���Ҫ���ݻ��嵽����
//		SOCKET_ERROR		: ����ʧ��
//		0					: ���ͳɹ�
int CClientSocket::DoSend(WSABUF& wsabuf,DWORD& dwByteSent)
{
	//WSAOVERLAPPED	Overlapped;
	//Overlapped.hEvent = NULL;//(WSAEVENT)m_hOverlapedEvent;
	//int retv = WSASend(m_SocketClient,&wsabuf,1,&dwByteSent,0,&Overlapped,NULL);
	//int retv = WSASend(m_SocketClient,&wsabuf,1,&dwByteSent,0,NULL,SendCompletionROUTINE);
	//int retv = WSASend(m_SocketClient,&wsabuf,1,&dwByteSent,0,NULL,NULL);
	//............

	int retv = send(m_SocketClient,wsabuf.buf,wsabuf.len,0);
	if(retv == SOCKET_ERROR)
	{
		int nErrorCode = WSAGetLastError();
		if(nErrorCode == WSA_IO_PENDING)// �Ѿ��ݽ���socket��Ļ����У���δ�������ͳ�ȥ
			return 0;
		else if(nErrorCode == WSAEWOULDBLOCK)
		{
#ifdef _DEBUG
			OutputDebugString(L"Warn : CClientSocket::DoSend() : WSAEWOULDBLOCK\r\n");
#endif
			return WSA_IO_INCOMPLETE;	// ���β��ܷ��ͳ�ȥ���ȴ�FD_WRITE�¼������ٷ���
		}
		else
		{
#if defined(_DEBUG)
			OutputDebugString(L"��������ʧ��\r\n");
#endif
			return SOCKET_ERROR;
		}
	}
	else 
		dwByteSent = (DWORD)retv;
	return 0;
}
//------------------------------------------------------------------------
// �����ݷ��ͣ������������ݷ񣿼�ⷢ�ͻ������Ƿ������ݷ���
// �յ�FD_WRITE�¼�������Ƿ���������Ҫ����
void CClientSocket::OnFD_WRITE()
{
	if(!IsConnected())
		return;
	m_LockClientSocketContext.Lock();
	while(!m_SendQueue.empty())
	{
		WSABUF& wsabuf = m_SendQueue.front();
		DWORD dwByteSent = 0;
		DWORD dwReturn = DoSend(wsabuf,dwByteSent);
		if(dwReturn == 0)
		{ // ���ͳɹ�
			if(dwByteSent < wsabuf.len)
			{ // û�з�����ɣ��˽�㲻ɾ��
				if(dwByteSent > 0)
				{
					memmove(wsabuf.buf,wsabuf.buf + dwByteSent,wsabuf.len - dwByteSent);
					wsabuf.len -= dwByteSent;
				}
				break; // �˴�û��ȫ�������꣬���ٷ���
			}
			else
			{
				delete wsabuf.buf;
				m_SendQueue.pop_front();
			}
		}
		else if(dwReturn == WSA_IO_INCOMPLETE) // socket����WSAEWOULDBLOCK�������ٷ��ȴ��´λ���
			break;
		else if(dwReturn == SOCKET_ERROR)
		{// ����ʧ��
			m_LockClientSocketContext.Unlock();
			ShowSocketError("��������ʧ��",WSAGetLastError());
			return;
		}
	}
	m_LockClientSocketContext.Unlock();
}
//------------------------------------------------------------------------
void CClientSocket::ClearSocketData()
{
	m_LockClientSocketContext.Lock();
	m_ConnectState = stateClosed;
	if(m_SocketClient != INVALID_SOCKET)
	{
		closesocket(m_SocketClient);
		m_SocketClient = INVALID_SOCKET;
	}
	while(m_SendQueue.size () != 0)
	{
		WSABUF& wsabuf = m_SendQueue.front();
		if(wsabuf.buf)
			delete wsabuf.buf;
		m_SendQueue.pop_front();
	}
	m_LockClientSocketContext.Unlock();	
}
//------------------------------------------------------------------------
BOOL CClientSocket::DoConnect(LPCSTR szServerAddress, WORD wServerPort, NET_PROXY_OPTION *pProxyOption)
{
	struct sockaddr_in ServerAddress;
	memset(&ServerAddress,0,sizeof(ServerAddress));
	if(!GetIP(m_szServerAddress,&ServerAddress.sin_addr))
		return FALSE;
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(m_wServerPort);

	struct sockaddr_in ProxyServerAddress;
	memset(&ProxyServerAddress,0,sizeof(ProxyServerAddress));
	int retv = 0;

	try
	{
		m_SocketClient = WSASocket(AF_INET ,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
		if(m_SocketClient == INVALID_SOCKET)
			throw "WSASocket() error";
		
		BOOL bAlive = TRUE;
		if(setsockopt(m_SocketClient,SOL_SOCKET,SO_KEEPALIVE,(char*)&bAlive,sizeof(BOOL)))
		{
			closesocket(m_SocketClient);
			TraceLn("CListenThread::Start error,setsockopt failed");
			return FALSE;
		}
		
		int nBufSize=65535;//����ϵͳ������
		if(setsockopt(m_SocketClient,SOL_SOCKET,SO_SNDBUF,(char*)&nBufSize,sizeof(int)))
		{
			closesocket(m_SocketClient);
			TraceLn("CListenThread::Start error,setsockopt failed");
			return FALSE;
		}

		if(!m_ProxyOption.bEnableProxy)
		{
			retv = WSAConnect(m_SocketClient , (struct sockaddr*)&ServerAddress,sizeof(ServerAddress),NULL,NULL,NULL,NULL);
			TraceLn("!!!WSAConnect!!! CClientSocket::DoConnect  1  IP = %s port = %d", szServerAddress, wServerPort);
			if(retv == SOCKET_ERROR)
			{
				if(WSAGetLastError() != WSAEWOULDBLOCK)
				{
					ShowConnectError("�޷����ӷ�����",WSAGetLastError());
					return FALSE;
				}
			}
			int retv = WSAEventSelect(m_SocketClient,m_hRefCommEvent,FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
			if(retv != 0)
				throw "WSAEventSelect() error";
			return TRUE;
		}
		
	}
	catch(char* szMsg)
	{
		ShowConnectError(szMsg,0);
		return FALSE;
	}

	// ���������ģʽ
	if(!GetIP(m_ProxyOption.szProxyServerName,&ProxyServerAddress.sin_addr))
		return FALSE;

	ProxyServerAddress.sin_family = AF_INET;
	ProxyServerAddress.sin_port = htons(m_ProxyOption.wProxyPort);

	WSASetLastError(0);
	DWORD arg = 0;
	retv = ioctlsocket(m_SocketClient,FIONBIO,&arg);		// �д��������ʱ��ΪBlocking��ʽ
	if(retv != 0)
	{
		ShowConnectError("ioctlsocket() error",WSAGetLastError());
		return FALSE;
	}
	retv = WSAConnect(m_SocketClient , (struct sockaddr*)&ProxyServerAddress,sizeof(ProxyServerAddress),NULL,NULL,NULL,NULL);
	TraceLn("!!!WSAConnect!!! CClientSocket::DoConnect  2  IP = %s port = %d", szServerAddress, wServerPort);
	if(retv == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			ShowConnectError("�������������ӳ���",0);
			return FALSE;
		}
	}

	WSASetLastError(0);	// ��յ�ǰSOCKET�ĳ�����Ϣ

	// ����Ϊ��socks4��socks5ѡ��ʱ���������������ͨѶ
	char buf[4096];
	int nOff = 0;
	if(m_ProxyOption.socksType == proxy5)
	{ // 1������������������ʽ�������������뷽ʽ�������뷽ʽ
		//DebugMsgLn("Connect '%s' via proxy server '%s' by sock5",m_szServerAlias,m_szProxyServer);
		buf[nOff++] = 5;	// ver
		buf[nOff++] = 1;	// method count
		buf[nOff++] = m_ProxyOption.szProxyUserName[0] == 0 ? 0 : 2;	// method
		DumpData("Reqest methods : ",buf,nOff);
		if(send(m_SocketClient,buf,nOff,0) != nOff)
		{
			ShowConnectError("������������ʧ��",0);
			return FALSE;
		}
		if(recv(m_SocketClient,buf,2,0) != 2)
		{
			ShowConnectError("���մ��������Ӧ��ʧ��",0);
			return FALSE;
		}
		DumpData("Server reply Reqest methods : ",buf,2);
		if(buf[0] != 5)
		{
			ShowConnectError("���մ��������Ӧ��ʧ��,Error 1",MSG_ERROR_MSG);
			return FALSE;
		}
		if(buf[1] != 0 && buf[1] != 2)
		{
			ShowConnectError("����������ܾ�����",MSG_ERROR_MSG);
			return FALSE;
		}
		int nOff = 0;
		if(buf[1] == 2)
		{
			// 2�������֤����
		   /*+----+------+----------+------+----------+
		   |VER | ULEN |  UNAME   | PLEN |  PASSWD  |
		   +----+------+----------+------+----------+
		   | 1  |  1   | 1 to 255 |  1   | 1 to 255 |
		   +----+------+----------+------+----------+*/
			// version of the subnegotiation = 1
			buf[nOff++] = 1; 
			int nStrLen = strlen(m_ProxyOption.szProxyUserName);
			buf[nOff++] = (char)nStrLen;
			memcpy(buf + nOff,m_ProxyOption.szProxyUserName,nStrLen);
			nOff += nStrLen;

			nStrLen = strlen(m_ProxyOption.szProxyPassword);
			buf[nOff++] = (char)nStrLen;
			memcpy(buf + nOff,m_ProxyOption.szProxyPassword,nStrLen);
			nOff += nStrLen;

			DebugMsgLn(L"Request authority : User(%s) and Password(%s)",m_ProxyOption.szProxyUserName,m_ProxyOption.szProxyPassword);

			if(send(m_SocketClient,buf,nOff,0) != nOff)
			{
				ShowConnectError("�������������������֤ʧ��",0);
				return FALSE;
			}

			if(recv(m_SocketClient,buf,2,0) != 2)
			{
				ShowConnectError("���մ�������������֤Ӧ��ʧ��",MSG_ERROR_MSG);
				return FALSE;
			}
			DumpData("Server reply Reqest authority : ",buf,2);
			
			// 3��proxy Ӧ�������֤����
			/*+----+--------+
			|VER | STATUS |
			+----+--------+
			| 1  |   1    |
			+----+--------+*/
			// version of the subnegotiation = 1
			if(buf[0] != 1)
			{
				ShowConnectError("����ͨ���������������ݿ����д�",MSG_ERROR_MSG);
				return FALSE;
			}
			if(buf[1] != 0)
			{
				ShowConnectError("������������ͨ�������������û���������",MSG_ERROR_MSG);
				return FALSE;
			}
		}
		
		// 2����������Զ�̷�����
		/*+----+-----+-------+------+----------+--------+
		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
		+----+-----+-------+------+----------+----------+
		| 1  |  1  | X'00' |  1   | Variable |    2     |
		+----+-----+-------+------+----------+----------*/
		nOff = 0;
		buf[nOff++] = 5;	// ver
		buf[nOff++] = 1;	// connect;
		buf[nOff++] = 0;	// RSV
		buf[nOff++] = 1;	// Adress type : IP V4
		memcpy(buf + nOff,(char*)&ServerAddress.sin_addr,4);
		nOff += 4;
		*(WORD*)(buf + nOff) = ServerAddress.sin_port;
		nOff += 2;

		DumpData("Reqest connect : ",buf,nOff);
		if(send(m_SocketClient,buf,nOff,0) != nOff)
		{
			ShowConnectError("��������������Զ������ʧ��",0);
			return FALSE;
		}
		
		// ����PROXY��connect��reply
		// reply�����ݽṹ��connect�����һ��
		if(recv(m_SocketClient,buf,4,0) != 4)
		{
			ShowConnectError("���մ���������������ӵ�Ӧ��ʧ��",0);
			return FALSE;
		}
		DumpData("Server reply Reqest connect : ",buf,4);

		if(buf[0] != 5)
		{
			ShowConnectError("���մ���������������ӵ�Ӧ��ʧ��,Error 1",MSG_ERROR_MSG);
			return FALSE;
		}
		if(buf[1] != 0)
		{
			ShowConnectError("����������ܾ���Զ�����ӵ�����",MSG_ERROR_MSG);
			return FALSE;
		}

		int nLeftDataLen = buf[3] == 1 ? 6 : buf[4] + 2;
		if(recv(m_SocketClient,buf,nLeftDataLen,0) != nLeftDataLen)
		{
			ShowConnectError("���մ�����������ݹ����г����쳣,Error 1",0);
			return FALSE;
		}
		DumpData("Server reply Reqest connect, other data : ",buf,nLeftDataLen);
	}
	else if(m_ProxyOption.socksType == proxy4)
	{
	   /*----+----+----+----+----+----+----+----+----+----+....+----+
		|VER | CMD| DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----*/
		//DebugMsgLn("Connect '%s' via proxy server '%s' by sock4 on port = %d",m_szServerName,m_szProxyServer,m_wProxyPort);
		int nOff = 0;
		buf[nOff++] = 4;	// VER
		buf[nOff++] = 1;	// connect
		*(WORD*)(buf + nOff) = ServerAddress.sin_port;
		nOff += 2;
		memcpy(buf + nOff,(char*)&ServerAddress.sin_addr,4);
		nOff += 4;
		buf[nOff++] = 0;	// no userid

		DumpData("Reqest connect : ",buf,nOff);
		if(send(m_SocketClient,buf,nOff,0) != nOff)
		{
			ShowConnectError("��������������Զ������ʧ��",0);
			return FALSE;
		}
		if(recv(m_SocketClient,buf,8,0) != 8)
		{
			ShowConnectError("���մ���������������ӵ�Ӧ��ʧ��",0);
			return FALSE;
		}
		DumpData("Server reply Reqest connetct : ",buf,8);
		if(buf[0] != 0)		// the version of the reply code
		{
			ShowConnectError("���մ���������������ӵ�Ӧ��ʧ��",MSG_ERROR_MSG);
			return FALSE;
		}
		if(buf[1] != 90)	// request granted
		{
			ShowConnectError("����������ܾ����ӻ�����ʧ��",MSG_ERROR_MSG);
			return FALSE;
		}
	}

	// 3����Ϊ������NonBlocking��ʽ
	WSAEventSelect(m_SocketClient , m_hRefCommEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	return TRUE;
}
//------------------------------------------------------------------------