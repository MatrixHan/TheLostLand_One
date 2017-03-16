/**
*	created:		2013-4-16   5:21
*	filename: 		BaseServer
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include "../Include/baseserver.h"
#include "../Include/tempclient.h"
#include "../Include/socketfunction.h"
//------------------------------------------------------------------------
CBaseServer::CBaseServer(void):m_xQTempClient( 10000 )
{
	m_pServerName = NULL;
	m_Type = ST_UNKNOWN;
	m_Ident.dwId = 0;
	m_iDBServerCount = 0;
	GetLocalTime(&m_stStartTime);
	m_bStateInfoUpdated = FALSE;
	m_fLoopTime = 0.0f;
	m_dwLoopTimes = 0;
	m_dwLoopStartTime = 0;
	m_dwConnectedTimes = 0;
	m_dwDisconnectTimes = 0;
	m_dwLoopTimesAll = 0;
	m_dwPreConnections = 0;
	m_xIocpServer.setEventListener(this);
	this->m_pIoConsole = NULL;
	srand( timeGetTime() );
	this->m_szBanIpFile[0] = 0;
	this->m_szTrustIpFile[0] = 0;
}
//------------------------------------------------------------------------
CBaseServer::~CBaseServer(void)
{

}
//------------------------------------------------------------------------
// ����������
BOOL CBaseServer::Start( CSettingFile & sf )
{
	//	��ȡ����������, m_pServerName�ڳ�ʼ��д��
	m_Ident.bType =(BYTE) GetServerTypeByName( sf.GetString( m_pServerName, "����������" ));
	if( m_Ident.bType == ST_UNKNOWN )
	{
		m_pIoConsole->OutPut( RGB(255,0,0), "�õ�һ��δ֪���������ͣ����������ļ�!\n" );
		return FALSE;
	}
	//	��ȡ����
	SetNickName( sf.GetString( m_pServerName, "��������", "����������" ) );
	//	��ȡ���������ĵĵ�ַ
	char szIpAddr[32];
	memset( szIpAddr, 0, 32 );
	m_SCAddr.nPort = (UINT)sf.GetInteger( NULL, "���ķ������˿�", 0 );
	if( !GetAddrByName( sf.GetString( NULL, "���ķ�����IP", "127.0.0.1" ), szIpAddr ))
		strcpy( szIpAddr, "127.0.0.1" );
	m_SCAddr.SetAddr( szIpAddr );
	//	ȡ�÷������ĵ�ַ�Ͷ˿�
	m_ServerAddr.nPort = (UINT)sf.GetInteger( m_pServerName, "�˿�", 0 );
	if( !GetAddrByName( sf.GetString( m_pServerName, "IP��ַ", "127.0.0.1" ), szIpAddr ))
		strcpy( szIpAddr, "127.0.0.1" );
	m_ServerAddr.SetAddr( szIpAddr );
	m_pIoConsole->OutPut( RGB(52,55,230), "��������������...\n" );

	char * pszIpFile = (char*)sf.GetString( m_pServerName, "��ֹIP��·��", "BanIP.txt" );
	o_strncpy( m_szBanIpFile, pszIpFile, 250 );
	this->m_BanList.Load( pszIpFile );
	pszIpFile = (char*)sf.GetString( m_pServerName, "���IP��·��", "TrustIP.txt" );
	o_strncpy( m_szTrustIpFile, pszIpFile, 250 );
	this->m_TrustList.Load( pszIpFile );
	m_pIoConsole->OutPut( RGB(52,55,230), "��ȡ���ñ����...\n" );

	//	����������
	BOOL	bRet = FALSE;
	bRet = m_xIocpServer.start();
	if( bRet )
	{
		bRet = m_xIocpServer.postListen( "0.0.0.0", m_ServerAddr.nPort, 64, 100000 );
		if( bRet )
		{
			if( !InitServer(sf) )
				bRet = FALSE;
			m_dwLoopStartTime = timeGetTime();
			if( !xThread::Start( this ) )
				bRet = FALSE;
		}
	}
	else
	{
		m_pIoConsole->OutPut( 255, m_xIocpServer.getErrorMsg() );
	}
	if( bRet )
	{
		m_pIoConsole->OutPut( RGB( 52,55,230), "�����������ɹ�...\n" );
	}
	else
	{
		m_pIoConsole->OutPut( 255, "(%d) %s\n", m_xIocpServer.getErrorCode(), m_xIocpServer.getErrorMsg());
	}

	return bRet;
}
//------------------------------------------------------------------------
VOID CBaseServer::GetServerState( SERVERSTATE & state )
{
	state.bUpdated = m_bStateInfoUpdated;
	m_bStateInfoUpdated = FALSE;
	SYSTEMTIME	stNow;
	GetLocalTime( &stNow );
	state.dwRunningSeconds = GetT1toT2Second(m_stStartTime, stNow );
	state.bUpdated = m_bStateInfoUpdated;
	state.dwRecvBytes = m_xIocpServer.getRecvKBytes();
	state.dwSendBytes = m_xIocpServer.getSendKBytes();
	state.fLoopTime = m_fLoopTime;
	state.numConnected = (int)m_dwConnectedTimes;
	state.numDisconnected = (int)m_dwDisconnectTimes;
	state.numConnection = m_dwConnectedTimes - m_dwDisconnectTimes;
	state.dwLoopTimes = m_dwLoopTimesAll;
	sprintf( m_szServerState, "%s-����ʱ��: %d��%dСʱ%d��%d��",
		m_szNickName, state.dwRunningSeconds / 86400, (state.dwRunningSeconds / 3600)%24, (state.dwRunningSeconds/60)%60, 
		state.dwRunningSeconds % 60 );
	state.pServerDescript = m_szServerState;
}
//------------------------------------------------------------------------
BOOL CBaseServer::Stop()
{
	xThread::Terminate();
	xThread::WaitFor();
	//	���������������
	CleanServer();
	m_SCClientObj.close();

	//	��������ײ�
	//CSocketIoManager::End();
	m_xIocpServer.stop();
	//	����Ͽ����ӵĶ���
	m_DisconnectQueue.Clean();
	return TRUE;
}
//------------------------------------------------------------------------
VOID CBaseServer::Execute(LPVOID lpParam )
{
	while( !IsTerminated() )
	{
		Sleep( 1 );
		m_xIocpServer.update();
		//	�������ӺͶϿ����ӵ��¼�~
		ProcConnectionEvent();
		//	��ѭ��
		Update();
		UpdateTotalInfo();
	}
}
//------------------------------------------------------------------------
BOOL CBaseServer::ConnectSCServer()
{
	if( m_SCClientObj.IsConnected() )
		m_SCClientObj.close();

	m_SCClientObj.Clean();
	m_SCClientObj.setServer( &m_xIocpServer );
	m_SCClientObj.SetMsgProcessor( this );
	if( m_xIocpServer.postConnection( m_SCAddr.addr, m_SCAddr.nPort, m_SCClientObj ))//OpenConnection(m_SCClientObj,m_SCAddr.addr, m_SCAddr.nPort );
	{
		m_SCClientObj.postRecv();
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CBaseServer::ConnectDBServer()
{
	for( int i = 0;i < m_iDBServerCount;i ++ )
	{
		if( m_DBClientObjs[i].IsConnected() )
			continue;
		m_DBClientObjs[i].setServer(&m_xIocpServer);
		m_DBClientObjs[i].SetMsgProcessor( this );
		if( ! m_xIocpServer.postConnection( m_DBAddrs[i].addr, m_DBAddrs[i].nPort, m_DBClientObjs[i] ))
			return FALSE;
		m_DBClientObjs[i].postRecv();
		OnDBServerReady( i );
	}
	return TRUE;
}
//------------------------------------------------------------------------
VOID CBaseServer::UpdateSCServer()
{
	PLMIRMSG pmsg = NULL;
	if(m_dwPreConnections != (m_dwConnectedTimes-m_dwDisconnectTimes) )
	{
		m_dwPreConnections = (m_dwConnectedTimes-m_dwDisconnectTimes);
		m_SCClientObj.UpdateInfo(m_dwPreConnections);
	}
	if( m_SCClientObj.IsConnected())
	{
		m_SCClientObj.Update();
	}
	else
	{
		//	��������
		if( !ConnectSCServer() )
			m_pIoConsole->OutPut( RGB( 255, 0, 0 ), "���ӷ���������(%s:%u)ʧ��...\n",
			m_SCAddr.addr, m_SCAddr.nPort );
		else
		{
			m_SCClientObj.setId(ID_SERVERCENTERCONNECTION);
			m_SCClientObj.RegisterServer(m_ServerAddr, m_Ident, m_szNickName );
		}
	}
}
//------------------------------------------------------------------------
VOID CBaseServer::UpdateDBServer()
{
	PLMIRMSG pmsg = NULL;
	for( int i = 0;i < m_iDBServerCount;i ++ )
	{
		if( m_DBClientObjs[i].IsConnected())
		{
			m_DBClientObjs[i].Update();
		}
		else
		{
			//	��������
			if( !ConnectDBServer() )
				m_pIoConsole->OutPut( RGB( 255, 0, 0 ), "�������ݿ������ʧ��...\n" );
			else
			{
				ConnectDBInit();
				m_pIoConsole->OutPut( RGB(  0,255, 0 ), "�������ݿ�������ɹ�...\n" );
			}
		}
	}
}
//------------------------------------------------------------------------
VOID CBaseServer::ProcConnectionEvent()
{
	CTempClient * pTClient;
	int count = m_xQTempClient.getcount();

	for( int i = 0;i < count;i ++ )
	{
		pTClient = m_xQTempClient.pop();
		if( pTClient->IsTimeOut( 3000 ) )
		{
			pTClient->close();
			m_xTempClients.deleteObject( pTClient );
			continue;
		}
		m_xQTempClient.push( pTClient );
	}
}
//------------------------------------------------------------------------
VOID CBaseServer::Update()
{

}
//------------------------------------------------------------------------
VOID CBaseServer::OnSCMsg( PMIRMSG pMsg, int datasize )
{
	switch( pMsg->wCmd )
	{
	case SCM_MSGACROSSSERVER:
		{
			OnMASMsg( pMsg->wParam[0],pMsg->wParam[1],pMsg->wParam[2],pMsg->data, datasize );
		}
		break;
	case SCM_REGISTERSERVER:
		{
			if( pMsg->wParam[0] == SE_OK )
			{
				REGISTER_SERVER_RESULT * presult = (REGISTER_SERVER_RESULT *)pMsg->data;

				m_pIoConsole->OutPut( RGB( 255, 0, 255 ), "ע��������ɹ�,index = %d\n", 
					presult->Id.bIndex );
				m_Ident = presult->Id;
				m_iDBServerCount = presult->nDbCount;
				if( m_iDBServerCount > 2 )m_iDBServerCount = 2;
				for( int i = 0;i < m_iDBServerCount;i ++ )
				{
					m_DBAddrs[i] = presult->DbAddr[i];
					m_pIoConsole->OutPut( RGB( 255, 0, 255 ), "���ݿ������ %s:%d\n",
						m_DBAddrs[i].addr, m_DBAddrs[i].nPort );
				}
			}
			else
			{
				m_pIoConsole->OutPut( RGB( 255, 0, 255 ), "ע�������ʧ��,���� = %d\n", 
					pMsg->wParam[0] );
			}
		}
		break;
	}
}
//------------------------------------------------------------------------
VOID CBaseServer::UpdateTotalInfo()
{
	DWORD	dwCurTime = timeGetTime();
	m_dwLoopTimes++;
	m_dwLoopTimesAll++;
	if( m_dwLoopTimes >= 1000 )
	{
		m_dwLoopTimes = 0;
		m_fLoopTime = (dwCurTime-m_dwLoopStartTime)/1000.0f;
		m_dwLoopStartTime = dwCurTime;
		m_bStateInfoUpdated = TRUE;
	}
}
//------------------------------------------------------------------------
static char g_szTempOutputBuffer[17384];
//------------------------------------------------------------------------
VOID CBaseServer::OnUnknownMsg( PMIRMSG pMsg, int datasize )
{
	CIOConsole * pConsole = (CIOConsole*)GetIoConsole();
	char	 * szText = g_szTempOutputBuffer;
	int	tptr=0;
	pConsole->OutPut( RGB( 0, 255, 0 ), "UNKNOW MESSAGE %d\n", pMsg->wCmd );
	pConsole->OutPut( RGB( 0, 255, 0 ),  /*fplog,*/"0x%08x %04x %04x %04x %04x[", pMsg->dwFlag, pMsg->wCmd, pMsg->wParam[0],pMsg->wParam[1],pMsg->wParam[2] );
	int i;
	for(i = 0;i < datasize;i ++ )
	{
		if( ((BYTE)*(pMsg->data + i ) ) < 20 )
			szText[i] = '.';
		else
			szText[i] = *(pMsg->data + i );
	}
	szText[i] = 0;
	pConsole->OutPut( RGB( 0, 255, 0 ),  /*fplog,*/"%s", szText); 
	pConsole->OutPut( RGB( 0, 255, 0 ),  /*fplog,*/"]\n" );
	pConsole->OutPut( RGB( 0, 255, 0 ), /* fplog,*/"[" );
	for( int i = 0;i < datasize;i ++ )
	{
		pConsole->OutPut( RGB( 0, 255, 0 ),  /*fplog,*/"%02x ", (BYTE)((*(pMsg->data + i))&0xff)); 
	}
	pConsole->OutPut( RGB( 0, 255, 0 ),  /*fplog,*/"]\n" );
}
//------------------------------------------------------------------------
VOID CBaseServer::OnEvent( xEventSender * pSender, int iEvent, int iParam, LPVOID lpParam )
{
	switch( iEvent )
	{
	case xIocpServer::ISE_ONCONNECTION:
		{
			xTempClient * pTempClient = (xTempClient*)lpParam;

			if( !(m_TrustList.IsEmpty() || m_TrustList.AddressIn( pTempClient->getAddress() ) ) )
			{
				m_pIoConsole->OutPut( RGB( 0, 255, 0 ), "������IP %s ���ӱ���ֹ...\n", pTempClient->getAddress() );
			}
			else if( !m_BanList.IsEmpty() && m_BanList.AddressIn( pTempClient->getAddress() ) )
			{
				m_pIoConsole->OutPut( RGB( 0, 255, 0 ), "����ֹIP %s ���ӱ���ֹ...\n", pTempClient->getAddress());
			}
			else
			{
				CClientObject * pObj = NewClientObject();
				if( pObj != NULL )
				{
					pObj->setServer( &m_xIocpServer );
					if( !pObj->steelSocket( *(xSocket*)lpParam ) )
					{
						DeleteClientObject(pObj);
					}
					else
					{
						pObj->OnConnection();
						m_pIoConsole->OutPut( RGB( 0, 255, 255 ), "����������( %s )...\n", pObj->getAddress());
						m_dwConnectedTimes++;
						pObj->postRecv();
					}
				}
				else
				{
					// ������
				}
			}
		}
		break;
	case	xIocpServer::ISE_ONDISCONNECTION:
		{
			CClientObject * pObj = (CClientObject*)lpParam;


			UINT	id = pObj->getId();
			pObj->OnDisconnect();

			CClientObject * pObject = GetClientObject( id );
			if( pObject == NULL )
			{
				if( id == ID_SERVERCENTERCONNECTION )
				{
					m_pIoConsole->OutPut( RGB( 255, 0, 0 ), "�����������ʧȥ���ӣ�\n" );
				}
				else
				{
					GetTickCount();
				}
			}
			else if( pObject == pObj )
			{
				m_pIoConsole->OutPut( RGB( 255, 0, 0 ), "����( %s )�����ӶϿ�...\n", pObject->getAddress() );
				DeleteClientObject( pObject ); 

				m_dwDisconnectTimes++;
			}
			else
			{
				m_pIoConsole->OutPut( RGB( 255, 0, 0 ), "��������!\n", id );
			}
		}
		break;
	}
}
//------------------------------------------------------------------------
VOID CBaseServer::OnUnCodeMsg(  xClientObject * pObject, const char * pszMsg, int size )
{

}
//------------------------------------------------------------------------
VOID CBaseServer::OnCodedMsg(  xClientObject * pObject, PMIRMSG	pMsg, int datasize )
{
	if( pObject == &this->m_SCClientObj )
	{
		if( pMsg->dwFlag == 0 )
			OnSCMsg( pMsg, datasize );
		else
		{
			CClientObject * pObject = (CClientObject*)GetClientObject( pMsg->dwFlag);
			if( pObject != NULL )
				pObject->OnSCMsg_T(pMsg, datasize);
		}
	}
	else if( pObject == &this->m_DBClientObjs[0] || pObject == &this->m_DBClientObjs[1] ) 
	{
		if( pMsg->dwFlag == 0 )
			OnDBMsg(pMsg, datasize);
		else
		{
			CClientObject * pObject = GetClientObject(pMsg->dwFlag);
			if( pObject != NULL )
				pObject->OnDBMsg(pMsg, datasize);
		}
	}
}
//------------------------------------------------------------------------
VOID CBaseServer::OnInput( const char * pszString )
{
	char	szLine[256];
	o_strncpy( szLine, pszString, 250 );
	xStringsExtracter<16> cmd( szLine, " \t,", " \t" );
	if( cmd.getCount() > 0 )
	{
		if( stricmp( cmd[0], "���¼���BanList" ) == 0 )
		{
			this->m_BanList.Load( this->m_szBanIpFile );
			this->GetIoConsole()->OutPut( 0xff00, "BanList %s ���¼���!\n", m_szBanIpFile );
		}
		else if( stricmp( cmd[0], "���¼���TrustList" ) == 0 )
		{
			this->m_TrustList.Load( this->m_szTrustIpFile );
			this->GetIoConsole()->OutPut( 0xff00, "TrustList %s ���¼���!\n", m_szTrustIpFile );
		}
	}
}
//------------------------------------------------------------------------