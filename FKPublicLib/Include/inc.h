/**
*	created:		2013-4-16   5:08
*	filename: 		inc
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define	WIN32_LEAN_AND_MEAN
//------------------------------------------------------------------------
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "winmm.lib")
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
//------------------------------------------------------------------------
#include "..\..\FKxLib\xlib.h"
#include "Gamedefine.h"
//------------------------------------------------------------------------
typedef union tagServerId
{
	struct
	{
		BYTE	bType;	//	����������	�μ� servertype
		BYTE	bGroup;	//	��������
		BYTE	bId;	//	������Ψһ��ʶ
		BYTE	bIndex;	//	�������ڷ��������ĵ�ע��˳��(�ɷ��������ĸ�����д)
	};
	DWORD 	dwId;
}ServerId;
//------------------------------------------------------------------------
enum	servertype
{
	ST_UNKNOWN = 0,
	ST_SERVERCENTER,	// ���ķ�����
	ST_DATABASESERVER,	// ���ݿ������
	ST_LOGINSERVER,		// ��½������
	ST_SELCHARSERVER,	// ѡ�˷�����
	ST_GAMESERVER,		// ��Ϸ������
};
//------------------------------------------------------------------------
inline	servertype GetServerTypeByName( const char * pszName )
{
	if( pszName == NULL )return ST_UNKNOWN;
	if( stricmp( pszName, "����������" ) == 0 )return ST_SERVERCENTER;
	if( stricmp( pszName, "���ݿ������" ) == 0 )return ST_DATABASESERVER;
	if( stricmp( pszName, "��½������" ) == 0 )return ST_LOGINSERVER;
	if( stricmp( pszName, "ѡ�˷�����" ) == 0 )return ST_SELCHARSERVER;
	if( stricmp( pszName, "��Ϸ���������" ) == 0 )return ST_GAMESERVER;
	return ST_UNKNOWN;
}
//------------------------------------------------------------------------
inline const char * GetServerTypeNameByType( servertype type )
{
	switch( type )
	{
	case	ST_SERVERCENTER:
		return "����������";
	case	ST_DATABASESERVER:
		return "���ݿ������";
	case	ST_LOGINSERVER:
		return "��½������";
	case	ST_SELCHARSERVER:
		return "ѡ�˷�����";
	case	ST_GAMESERVER:
		return "��Ϸ���������";
	}
	return "δ֪�ķ���������";
}
//------------------------------------------------------------------------
// ��������ַ
typedef struct tagSERVERADDR
{
	tagSERVERADDR()
	{
		memset( this, 0, sizeof( *this));
	}
	char	addr[16];
	DWORD	nPort;
	void	SetAddr( const char * pAddr )
	{
		strncpy( addr, pAddr, 16 );
		addr[15] = 0;
	}

}SERVERADDR;
//------------------------------------------------------------------------
// ��ĳ���������з�����ע��ʱ��ע����Ϣ
typedef struct tagREGISTER_SERVER_INFO
{
	char	szName[64];
	ServerId	Id;
	SERVERADDR	addr;
}REGISTER_SERVER_INFO;
//------------------------------------------------------------------------
// �������ע������һ���������Ľ��
typedef struct tagREGISTER_SERVER_RESULT
{
	ServerId	Id;
	int			nDbCount;
	SERVERADDR	DbAddr[2];
}REGISTER_SERVER_RESULT;
//------------------------------------------------------------------------
// ����ĳһ��ע�����������Ϣ
typedef struct tagFINDSERVER_RESULT
{
	tagFINDSERVER_RESULT()
	{
		ZeroMemory( this, sizeof(*this));
	}
	SERVERADDR	addr;
	ServerId	Id;
}FINDSERVER_RESULT;
//------------------------------------------------------------------------
// ������������
typedef enum e_servererror
{
	SE_FAIL,
	SE_OK,
	SE_SERVERFULL,					// ����������
	SE_INVALIDPARAM,				// ע�������ʧ�ܣ����������ʹ���
	SE_NOTINITLIZED,				// ж��ע�������ʧ�ܣ��÷�����û��ע���
	SE_SERVERNOTFOUND,				// ���ҷ�����ʧ��
	SE_ALLOCMEMORYFAIL,				// �����������ڴ�ʧ��
	SE_ODBC_SQLALLOCHANDLEFAIL,
	SE_ODBC_SQLSETENVATTRFAIL,
	SE_ODBC_SQLCONNECTFAIL,
	SE_ODBC_SQLDISCONNECTFAIL,
	SE_ODBC_SQLEXECDIRECTFAIL,
	SE_ODBC_SQLNUMRESULTCOLSFAIL,	// NumResultCols
	SE_ODBC_SQLDESCRIBECOLFAIL,		// DescribeCol
	SE_ODBC_SQLBINDCOLFAIL,
	SE_ODBC_SQLFETCHFAIL,
	SE_ODBC_SQLPREPAREFAIL,
	SE_ODBC_SQLEXECUTEFAIL,
	SE_DB_NOMOREDATA,
	SE_DB_NOTINITED,
	SE_LOGIN_ACCOUNTEXIST,
	SE_LOGIN_ACCOUNTINGAME,
	SE_LOGIN_TIMEDELAY,
	SE_LOGIN_ACCOUNTNOTEXIST,
	SE_LOGIN_ACCOUNTKEY_NOTEXIST,
	SE_LOGIN_ACCOUNT_CREATE_ERR,
	SE_LOGIN_PASSWORDERROR,
	SE_LOGIN_ACCOUNTONLINE,
	SE_SELCHAR_CHAREXIST,
	SE_SELCHAR_NOTEXIST,
	SE_REG_INVALIDACCOUNT,
	SE_REG_INVALIDPASSWORD,
	SE_REG_INVALIDNAME,
	SE_REG_INVALIDBIRTHDAY,
	SE_REG_INVALIDPHONENUMBER,
	SE_REG_INVALIDMOBILEPHONE,
	SE_REG_INVALIDQUESTION,
	SE_REG_INVALIDANSWER,
	SE_REG_INVALIDIDCARD,
	SE_REG_INVALIDEMAIL,
	SE_CREATECHARACTER_INVALID_CHARNAME,
}SERVER_ERROR;
//------------------------------------------------------------------------
// Login�������пͻ������״̬
enum	loginseverstate
{
	LST_NOTVERIFIED,			// δ��½
};
//------------------------------------------------------------------------
// SelcectChar�������еĿͻ������״̬
enum	selcharserverstate
{
	SCS_NOTVERIFIED,			// δ��½
	SCS_WAITINGFORVERIFIED,		// �ȴ���½����һ֡�Զ���½��
	SCS_VERIFIED,				// �ѵ�½
};
//------------------------------------------------------------------------
// Game�������еĿͻ������״̬
enum	gameserverusermode
{
	GSUM_NOTVERIFIED,			// δ��½
	GSUM_WAITINGDBINFO,			// �ȴ�DB����״̬
	GSUM_WAITINGCONFIRM,		// 
	GSUM_VERIFIED,				// �ѵ�½
};
//------------------------------------------------------------------------
#define ID_SERVERCENTERCONNECTION	100032
#define ID_DBSERVERCONNECTIONSTART	100033
//------------------------------------------------------------------------
enum	MAS_SENDTYPE
{
	MST_SINGLE,	//	����һ������������������Ϊ������������
	MST_GROUP,	//	����һ���������飬����Ϊ���������
	MST_TYPE,	//	����һ�������������Ϊ���������ͺ�
};
//------------------------------------------------------------------------
typedef struct tagENTERGAMESERVER
{
	tagENTERGAMESERVER()
	{
		memset( this, 0, sizeof( *this));
	}
	char	szAccount[12];
	UINT	nLoginId;
	UINT	nSelCharId;
	union
	{
		UINT			nClientId;
		SERVER_ERROR	result;
	};
	DWORD	dwEnterTime;
	char	szName[32];
	DWORD	dwSelectCharServerId;
}ENTERGAMESERVER;
//------------------------------------------------------------------------
#define SERVER_COPYRIGHT		"FreeKnight"
//------------------------------------------------------------------------