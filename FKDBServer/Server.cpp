/**
*	created:		2013-4-15   23:24
*	filename: 		Server
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\server.h"
#include "odbcdatabase.h"
#include "time.h"
#include "FKDBServerDefine.h"
//------------------------------------------------------------------------
CServer *CServer::m_pInstance = NULL;
//------------------------------------------------------------------------
CServer::CServer(void)
{
	m_pInstance = this;
	strcpy( m_szServer, "(local)" );
	strcpy( m_szDatabase, "FKGameDB" );
	strcpy( m_szAccount, "root" );
	strcpy( m_szPassword, "123456" );
	strcpy( m_szWebDatabase, "WebDB" );
	memset( m_dwMsgTimes, 0, sizeof( m_dwMsgTimes ) );
}
//------------------------------------------------------------------------
CServer::~CServer(void)
{

}
//------------------------------------------------------------------------
CClientObject * CServer::NewClientObject()
{
	CClientObj * pObj = newObject();
	if( pObj->GetAppDB().OpenDataBase( m_szServer, m_szDatabase, m_szAccount, m_szPassword ) != SE_OK )
	{
		m_pIoConsole->OutPut( RGB(255,0,0), "�޷��������ݿ⣬����������Ƿ���!\n" );
		deleteObject( pObj );
		return NULL;
	}
	return pObj;
}
//------------------------------------------------------------------------
VOID CServer::DeleteClientObject( CClientObject * pObject )
{
	deleteObject((CClientObj*)pObject );
}
//------------------------------------------------------------------------
// ��ʼ�����ݿ�
BOOL CServer::InitServer(CSettingFile&s)
{
	int maxconnection = s.GetInteger( m_pServerName, "���ݿ����������", MAX_CLIENTOBJECT );
	create( maxconnection );
	CBaseServer::GetIoConsole()->OutPut( 0xff00, "���ݿ���������� %d...\n", maxconnection );

	srand( time(NULL));
	char * pszServer = (char*)s.GetString( m_pServerName, "���ݿ��������", "(local)" );
	char * pszDatabase = (char*)s.GetString( m_pServerName, "�������ݿ���", "FKGameDB" );
	char * szAccount = (char*)s.GetString( m_pServerName, "���ݿ��˺�", "root" );
	char * szPassword = (char*)s.GetString( m_pServerName, "���ݿ�����", "123456" );
	char * pszWebDatabase = (char*)s.GetString( m_pServerName, "WEB���ݿ���", "WebDB" );

	strncpy( m_szServer, pszServer, 63 );
	strncpy( m_szDatabase, pszDatabase, 63 );
	strncpy( m_szAccount, szAccount, 63 );
	strncpy( m_szPassword, szPassword, 63 );
	strncpy( m_szWebDatabase, pszWebDatabase, 63 );

	CBaseServer::GetIoConsole()->OutPut( 0xff00, "�����������ݿ� SV=%s : DB=%s : ID=%s : PW=%s \n",
		pszServer, pszDatabase, szAccount, szPassword );

	// �������ݿ�
	if( m_appDB.OpenDataBase( pszServer, pszDatabase, szAccount, szPassword ) != SE_OK )
	{
		m_pIoConsole->OutPut( RGB(255,0,0), "�޷��������ݿ⣬����������Ƿ���!\n" );
		return FALSE;
	}
	else
	{
		m_pIoConsole->OutPut( RGB(0,255,0), "�������ݿ�ɹ�!\n" );
	}

	m_appDB.DoInit();
	m_xxShowTimer.Savetime();

	// Web���ݿ�
	if( m_appDB.OpenWebDB( pszServer, pszWebDatabase, szAccount, szPassword ) != SE_OK )
	{
		m_pIoConsole->OutPut( RGB(255,0,0), "�޷�����Web���ݿ� %s������������Ƿ���!\n", pszWebDatabase );
		return FALSE;
	}
	else
	{
		m_pIoConsole->OutPut( RGB(0,255,0), "����Web���ݿ�ɹ�!\n" );
	}

	return TRUE;
}
//------------------------------------------------------------------------
VOID CServer::CleanServer()
{

}
//------------------------------------------------------------------------
VOID CServer::Update()
{
	CClientObj * pObject = m_ObjectPool.First();
	while( pObject )
	{
		pObject->Update();
		pObject = m_ObjectPool.Next();
	}
	
	UpdateSCServer();
}
//------------------------------------------------------------------------
VOID CServer::OnParseMsg( WORD wMsg )
{
	if( wMsg >= DM_START && wMsg < DM_END )
	{
		this->m_dwMsgTimes[wMsg]++;
	}
}
//------------------------------------------------------------------------