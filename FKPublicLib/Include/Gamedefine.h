/**
*	created:		2013-4-15   22:51
*	filename: 		gamedefine
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "BaseMsgDefine.h"
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define DPRINT	CServer::GetInstance()->GetIoConsole()->OutPut
	#define DSYSMSG	SaySystem
#else
	inline void empty_func(...){}
	#define DPRINT	empty_func
	#define	DSYSMSG	empty_func
#endif
//------------------------------------------------------------------------
#define PRINT	CServer::GetInstance()->GetIoConsole()->OutPut
//------------------------------------------------------------------------
#define LOG		CLogFile::GetInstance()->LogTextRt
//------------------------------------------------------------------------
typedef struct tagColType
{
	WORD type;
	WORD length;
}ColType;
//------------------------------------------------------------------------
typedef struct tagExecSqlRecord
{
	DWORD dwCount;
	DWORD dwColCount;
	ColType coldef[1];
}ExecSqlRecord;
//------------------------------------------------------------------------
typedef struct tagExecSqlCommand
{
	DWORD dwLength;
	char	command[1];
}ExecSqlCommand;
//------------------------------------------------------------------------
enum eColType
{
	CT_STRING,			//	�ַ���
	CT_TINYINT,			//	8λ����
	CT_SMALLINT,		//	16λ����
	CT_INTEGER,			//	32λ����
	CT_BIGINT,			//	64λ����
	CT_DATETIME,		//	ʱ��
	CT_CODEDARRAY,		//	����������
};
//------------------------------------------------------------------------