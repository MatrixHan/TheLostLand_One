/**
*	created:		2013-4-18   23:36
*	filename: 		Server
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\server.h"
#include "gameworld.h"
#include ".\humanplayermgr.h"
#include "savetodbthread.h"
//------------------------------------------------------------------------
class CHumanPlayer;
//------------------------------------------------------------------------
CServer *CServer::m_pInstance = NULL;
//------------------------------------------------------------------------
CServer::CServer(void)
{
	m_pInstance = this;
	m_bWillClose = FALSE;
}
//------------------------------------------------------------------------
CServer::~CServer(void)
{
	GetTickCount();
}
//------------------------------------------------------------------------
CClientObject * CServer::NewClientObject()
{
	return newObject();
}
//------------------------------------------------------------------------
VOID CServer::DeleteClientObject( CClientObject * pObject )
{
	deleteObject( (CClientObj*)pObject );
}
//------------------------------------------------------------------------
BOOL CServer::InitServer(CSettingFile&s)
{
	int maxconnection = s.GetInteger( m_pServerName, "���������", MAX_CLIENTOBJECT );
	create( maxconnection );
	m_EnterObjects.Create( maxconnection );
	CBaseServer::GetIoConsole()->OutPut( 0xff00, "��������� %d!\n", maxconnection );
	m_pGameWorld = CGameWorld::GetInstance();
	return m_pGameWorld->Init();
}
//------------------------------------------------------------------------
VOID CServer::CleanServer()
{

}
//------------------------------------------------------------------------
VOID CServer::Update()
{
	try
	{
		CClientObj * pObject = m_ObjectPool.First();
		while( pObject )
		{
			pObject->Update();
			pObject = m_ObjectPool.Next();
		}
		
		UpdateSCServer();
		UpdateDBServer();
		m_pGameWorld->Update();
	}
	catch( ... )
	{
		// �洢�������� 
		SaveAllPlayerData();
		throw;
	}
}
//------------------------------------------------------------------------
VOID CServer::OnDBMsg( PMIRMSG pMsg, int datasize )
{

}
//------------------------------------------------------------------------
//SelCharServer����Ϣ
VOID CServer::OnMASMsg( WORD wCmd, WORD wType, WORD wIndex, char * pszData, int datasize )
{
	switch( wCmd )
	{
	case	MAS_ENTERGAMESERVER:
		{
			ENTERGAMESERVER * pEnterInfo = (ENTERGAMESERVER*)pszData;
			UINT id = pEnterInfo->nClientId;
			
			pEnterInfo->result = AddEnterAccount( pEnterInfo->nLoginId, pEnterInfo->nSelCharId, pEnterInfo->szAccount, pEnterInfo->szName, wIndex );
			m_SCClientObj.SendMsgAcrossServer( id, MAS_ENTERGAMESERVER, MST_SINGLE, wIndex, pszData, datasize );
		}
		break;
	}
}
//------------------------------------------------------------------------
//ServerCenter����Ϣ
VOID CServer::OnSCMsg(PMIRMSG pMsg, int datasize )
{
	switch( pMsg->wCmd )
	{
	case	SCM_FINDSERVER:
		{
			if( pMsg->wParam[0] == SE_OK )
			{
				SERVERADDR * pAddr = (SERVERADDR*)pMsg->data;
				m_pIoConsole->OutPut( RGB(255,0,0), "�ҵ��˷�������ַ %s:%d\n", pAddr->addr, pAddr->nPort );
			}
			else
				m_pIoConsole->OutPut( RGB(255,0,0), "û���ҵ������� %s �ĵ�ַ!\n", pMsg->data );
		}
		break;
	default:
		{
		   CBaseServer::OnSCMsg(pMsg, datasize );
		   switch( pMsg->wCmd )
		   {
		   case SCM_MSGACROSSSERVER:
			   {
				   OnMASMsg( pMsg->wParam[0],pMsg->wParam[1],pMsg->wParam[2],pMsg->data, datasize );
			   }
			   break;
		   }
		}
		break;
	}
}
//------------------------------------------------------------------------
void CServer::ConnectDBInit()
{

}
//------------------------------------------------------------------------
SERVER_ERROR CServer::AddEnterAccount( UINT nLoginId, UINT nSelCharId, const char * pszAccount, const char * pszName, WORD wIndex )
{
	ENTERGAMESERVER	*	pEnterInfo = NULL;
	UINT id = m_EnterObjects.New( &pEnterInfo);
	if( id == 0 || pEnterInfo == NULL )return SE_SERVERFULL;
	if( !m_Inthash.HAdd( nLoginId, id ) )
	{
		m_EnterObjects.Del(id);
		return SE_SERVERFULL;
	}
	pEnterInfo->nClientId = id;
	pEnterInfo->nLoginId = nLoginId;
	pEnterInfo->nSelCharId = nSelCharId;
	pEnterInfo->dwEnterTime = timeGetTime();
	strncpy( pEnterInfo->szAccount, pszAccount, 10 );
	o_strncpy( pEnterInfo->szName, pszName, 20 );
	pEnterInfo->szAccount[10] = 0;
	pEnterInfo->dwSelectCharServerId = wIndex;
	return SE_OK;
}
//------------------------------------------------------------------------
BOOL CServer::GetEnterInfo( UINT nLoginId, UINT nSelCharId, const char * pszAccount, ENTERGAMESERVER & enterinfo )
{
	UINT id = (UINT)m_Inthash.HGet(nLoginId);
	if( id == 0 )return FALSE;
	ENTERGAMESERVER * pEnterInfo = m_EnterObjects.Get(id);
	if( pEnterInfo == NULL )return FALSE;
	if( pEnterInfo->nLoginId != nLoginId || pEnterInfo->nSelCharId != nSelCharId )return FALSE;
	enterinfo = *pEnterInfo;
	m_EnterObjects.Del(id);
	m_Inthash.HDel(nLoginId);
	return TRUE;
}
//------------------------------------------------------------------------
static char m_szInputString[2048];
//------------------------------------------------------------------------
void	CServer::OnInput( const char * pString )
{
	PRINT(0xff,"������Ϣ��%s\n",pString);
	CBaseServer::OnInput( pString );
	
	m_pGameWorld->OnInput(pString);
}
//------------------------------------------------------------------------
VOID CServer::WillClose()
{
	// �����߼��߳�
	Suspend();
	// �洢�������� 
	SaveAllPlayerData();
	// ���¼����߼��߳�
	Resume();
}
//------------------------------------------------------------------------
VOID CServer::SaveAllPlayerData()
{
	int savecount = 0;
	CClientObj * pObject = m_ObjectPool.First();
	while( pObject )
	{
		pObject->EmergencySave();
		savecount ++;
		pObject = m_ObjectPool.Next();
	}
	if( savecount > 0 )
	{
		for( int i = 0;i < 5;i ++ )
		{
			UpdateDBServer();
		}
		FILE * fp = fopen( "exception_.log", "a+" );
		if( fp )
		{
			fprintf( fp, "[��������Ѿ�����]\n" );
			fclose( fp );
		}
	}
}
//------------------------------------------------------------------------
VOID CServer::KickAll()
{
	CClientObj * pObject = m_ObjectPool.First();
	while( pObject )
	{
		pObject->Disconnect( 2000 );
		pObject = m_ObjectPool.Next();
	}	
}
//------------------------------------------------------------------------
VOID CServer::OnTerminated(BOOL bExcepted)
{
	CSaveToDBThread thread;
	if( thread.Start() )
	{
		WaitForSingleObject( thread.getHandle(), INFINITE ); 
	}
}
//------------------------------------------------------------------------