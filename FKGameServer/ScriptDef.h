/**
*	created:		2013-4-18   23:07
*	filename: 		ScriptDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// ��ν����Ʒ����
typedef struct tagGoods
{
	tagGoods()
	{
		memset( this, 0, sizeof(*this));
	}
	char	szName[32];
	WORD	wCount;
	WORD	wRefreshTime;
	tagGoods * pNext;
}Goods;
//------------------------------------------------------------------------
// �ű���������
enum	script_param
{
	SP_DATAPOINTER,		//	����ָ��
	SP_INTEGER,			//	����
	SP_STRING,			//	�ַ���
	SP_VARIABLE,		//	����
};
//------------------------------------------------------------------------
// �ű�����
typedef struct tagScriptParamEx
{
	tagScriptParamEx()
	{
		memset( this, 0, sizeof( *this ) );
	}
	WORD wType;
	WORD wSize;
	char * pszParam;
	UINT	nParam;
}ScriptParamEx;
//------------------------------------------------------------------------
// �ص�����
typedef struct tagCallParamEx
{
	tagCallParamEx()
	{
		memset( this, 0, sizeof( *this ) );
		pszParam = "";
	}
	tagCallParamEx & operator =( tagCallParamEx & p1 )
	{
		nParam = p1.nParam;
		this->pszParam = p1.pszParam;
		if( p1.pszParam == p1.szbuffer )
		{
			o_strncpy( szbuffer, p1.szbuffer, 19 );
			this->pszParam = szbuffer;
		}
		else if( this->pszParam == NULL )
			this->pszParam = "";
		return *this;
	}
	tagCallParamEx & operator =( const char * szParam )
	{
		pszParam = (char*)szParam;
		if( szParam )
		{
			nParam = StringToInteger( szParam );
		}
		else
		{
			nParam = 0;
			szParam = "";
		}

		return *this;
	}

	tagCallParamEx & operator =( UINT val )
	{
		nParam = val;
		pszParam = szbuffer;
		sprintf( szbuffer, "%u", val );
		return *this;
	}

	char * pszParam;
	UINT nParam;
	char	szbuffer[20];
}CallParamEx;
//------------------------------------------------------------------------
class CScriptShell;
class CScriptTarget;
class CScriptView;
//------------------------------------------------------------------------
typedef DWORD (*fnCommandProc)( CScriptShell * pShell, CScriptTarget * pTarget, CScriptView * pView, CallParamEx * Params, UINT nParam, BOOL & bContinue );
//------------------------------------------------------------------------
typedef struct tagGetVariableStruct
{
	tagGetVariableStruct()
	{
		memset( this, 0, sizeof( *this ) );
	}
	void	SetValue( UINT nVal )
	{
		nType = 0;
		nValue = nVal;
	}
	void	SetValue( char * pszVal )
	{
		nType = 1;
		pszValue = pszVal;
	}
	void	SetValue( const char * pszVal )
	{
		nType = 1;
		pszValue = (char*)pszVal;
	}
	union
	{
		UINT	nValue;
		char	*	pszValue;
	};
	UINT	nType;
}GetVariableStruct;
//------------------------------------------------------------------------
class CHumanPlayer;
//------------------------------------------------------------------------
typedef BOOL (*fnGetVariable)( const char * pVarName, GetVariableStruct & var, CHumanPlayer * pPlayer );
//------------------------------------------------------------------------