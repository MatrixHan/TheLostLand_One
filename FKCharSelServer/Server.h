/**
*	created:		2013-4-15   22:47
*	filename: 		Server
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "clientobj.h"
#include "FKCharSelServerDefine.h"
//------------------------------------------------------------------------
class CServer :
	public CBaseServer,
	public xIndexObjectPool<CClientObj>
{
public:
	CServer(void);
	virtual ~CServer(void);
public:
	CClientObject * GetClientObject(UINT id ){ return getObject( id );}
	CClientObject * NewClientObject();
	VOID DeleteClientObject( CClientObject * pObject );
public:
	// ��ʼ��������
	BOOL InitServer(CSettingFile&);
	// ����������ڲ�����
	VOID CleanServer();
	// ÿ֡ˢ��
	VOID Update();
	// GameSvr��Ϣ
	VOID OnMASMsg( WORD wCmd, WORD wType, WORD wIndex,char * pszData, int datasize );
	// ���ķ�������Ϣ
	VOID OnSCMsg( PMIRMSG pMsg, int datasize );
	BOOL GetLoginEnterInfo( UINT nLoginId, UINT nSelCharId, LOGINENTER & info );
public:
	static CServer * GetInstance()
	{
		if( m_pInstance == NULL )
			m_pInstance = new CServer;
		return m_pInstance;
	}
protected:
	SERVER_ERROR	AddEnterAccount( UINT nLoginId, const char * pszAccount, UINT & nSelCharId );
	static CServer * m_pInstance;
	CIndexListEx<LOGINENTER/*, MAX_CLIENTOBJECT*/>	m_EnterObject;
	CIntHash<1024>	m_Inthash;
	CIndexListEx<CClientObj/*, MAX_CLIENTOBJECT*/>	m_ClientObjects;
};
//------------------------------------------------------------------------