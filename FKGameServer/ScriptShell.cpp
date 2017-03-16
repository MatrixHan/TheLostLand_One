/**
*	created:		2013-4-19   13:08
*	filename: 		ScriptShell
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\scriptshell.h"
#include ".\scripttarget.h"
#include ".\scriptobject.h"
#include ".\scriptview.h"
#include ".\scriptelement.h"
//------------------------------------------------------------------------
CScriptShell::CScriptShell(void)
{
	m_pScriptObject = NULL;
	m_ExecuteResult = ER_OK;
	memset( &m_dwResultValue, 0, sizeof( m_dwResultValue ) );
	m_pCallParams = NULL;
}
//------------------------------------------------------------------------
CScriptShell::~CScriptShell(void)
{
}
//------------------------------------------------------------------------
char * CScriptShell::GetVariableValue( const char * pszVariable )
{
	if( strnicmp( pszVariable, "param", 5 ) == 0 )
	{
		UINT index = (UINT)StringToInteger( pszVariable + 5 );
		if( index >= MAX_CALL_PARAMS )return NULL;
		if( this->m_pCallParams == NULL )return NULL;
		return this->m_pCallParams[index].pszParam;
	}
	else
	{
		char * pvalue = m_xVarList.GetVarValue( pszVariable );
		if( pvalue != NULL )
			return pvalue;
	}
	return xVariableProvider::GetVariableValue( pszVariable );
}
//------------------------------------------------------------------------
//	����������FALSE�󣬱�������������ӡ�
BOOL CScriptShell::Execute( CScriptTarget * pTarget, const char * pszPage1, BOOL bUserQuery, DWORD dwParam)
{
	CScriptPageView view(this);
	CParamStackHelper paramstack;

	char	szPage[1024];
	char * pszPage = NULL;
	CallParamEx params[MAX_CALL_PARAMS];
	setExecuteResult( ER_OK );
	if( m_pScriptObject == NULL )return FALSE;
	//	����ǵ�һ�ν����SHELL
	BOOL	bFirstPage = FALSE;
	if( pszPage1 == NULL )
	{
		//	������е�����
		pTarget->setCurScriptObject( NULL );
		pTarget->setCurShell( NULL );
		pszPage = "@main";
		bFirstPage = TRUE;
	}
	else
	{
		o_strncpy( szPage, pszPage1, 1020 );
		xStringsExtracter<MAX_CALL_PARAMS+1> queryparam( szPage, "&" );
		for( UINT n = 1;n < queryparam.getCount();n++ )
		{
			params[n-1] = queryparam[n];
			if( *params[n-1].pszParam == '$' )
			{
				char * p = pTarget->GetVariableValue( params[n-1].pszParam + 1 );
				if( p )
				{
					char * lparam = (char*)paramstack.getfreebuf();
					paramstack.push( (LPVOID)p, (int)strlen( p )+1 );
					params[n-1] = lparam;
				}
				else
					params[n-1] = "";
			}
		}
		if( queryparam.getCount() > 1 )
			this->m_pCallParams = params;
		pszPage = (char*)szPage;
	}
	//	�����ǰSHELL���Ǳ�SHELL
	if( pTarget->getCurShell() != this )
	{
		//	���ӵ���SHELL�����ҳ������
		pTarget->setCurScriptObject( NULL );
		pTarget->setCurShell( this );
	}

	//	��ȡ�����ҳ��
	CSe_Page * pPage = NULL;
	
	//	�����ǰ�ű����Ǳ���SHELL�Ľű�
	if( pTarget->getCurScriptObject() && pTarget->getCurScriptObject() != this->m_pScriptObject )
	{
		//	��Զ�̽ű�ȡ�������ҳ��
		pPage = pTarget->getCurScriptObject()->GetPage( pszPage );
	}
	else
		//	�ӱ��ؽű�ȡ�������ҳ��
		pPage = m_pScriptObject->GetPage( pszPage );
	if( pPage == NULL )
		return FALSE;
	//	���ҳ��û�з���Ȩ�ޣ����޷�����ҳ��
	if( bUserQuery && pPage->getAccessRight() != PAR_PUBLIC )
		return FALSE;
	//	����Ϊ��ǰҳ��
	pTarget->setCurScriptObject( pPage->getObject() );

	//	ִ��ҳ��
	if( !pPage->Execute( this, pTarget, &view ) )
	{
		//	ִ�н���ǹر�ҳ�棬�͹ر�ҳ��
		if( getExecuteResult() == ER_CLOSE )
		{
			pTarget->SendClosePage( this );
			return TRUE;
		}
	}
	//	���������������PAGEID�Ĵ���
	if( !OnPageShow( pTarget, &view, pszPage ) )
		return FALSE;
	//	���򣬷���ҳ��
	view.SendPageToTarget( pTarget, dwParam); 
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CScriptShell::Execute( CScriptTarget * pTarget, CSe_Page * pPage, BOOL bUserQuery, DWORD dwParam)
{	
	CScriptPageView view(this);
	CParamStackHelper paramstack;

	setExecuteResult( ER_OK );

	if( pPage == NULL )
		return FALSE;
	//	���ҳ��û�з���Ȩ�ޣ����޷�����ҳ��
	if( bUserQuery && pPage->getAccessRight() != PAR_PUBLIC )
		return FALSE;
	//	����Ϊ��ǰҳ��
	pTarget->setCurScriptObject( pPage->getObject() );

	//	ִ��ҳ��
	if( !pPage->Execute( this, pTarget, &view ) )
	{
		//	ִ�н���ǹر�ҳ�棬�͹ر�ҳ��
		if( getExecuteResult() == ER_CLOSE )
		{
			pTarget->SendClosePage( this );
			return TRUE;
		}
	}
	view.SendPageToTarget( pTarget, dwParam);
	return TRUE;
}
//------------------------------------------------------------------------