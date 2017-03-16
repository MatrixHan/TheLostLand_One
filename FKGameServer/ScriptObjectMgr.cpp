/**
*	created:		2013-4-19   9:43
*	filename: 		ScriptObjectMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\scriptobjectmgr.h"
#include "scriptobject.h"
//------------------------------------------------------------------------
CScriptObjectMgr * CScriptObjectMgr::m_pInstance = NULL;
//------------------------------------------------------------------------
CScriptObjectMgr::CScriptObjectMgr(void) : m_xScriptList( TRUE )
{
	if( m_pInstance == NULL )
		m_pInstance = this;
}
//------------------------------------------------------------------------
CScriptObjectMgr::~CScriptObjectMgr(void)
{
}
//------------------------------------------------------------------------
VOID	CScriptObjectMgr::Load( const char * pszPath )
{
	StartFind( pszPath, "*.txt", TRUE, FINDFILE_SCRIPT );
	StartFind( pszPath, "*.def", TRUE, FINDFILE_DEFINE );
}
//------------------------------------------------------------------------
VOID	CScriptObjectMgr::OnFoundFile( const char * pszFilename, UINT nParam )
{
	if( nParam == FINDFILE_DEFINE )
	{
		LoadDefine( pszFilename );
	}
	else if( nParam == FINDFILE_SCRIPT )
	{
		LoadScript( pszFilename );
	}
}
//------------------------------------------------------------------------
char * CScriptObjectMgr::GetVariableValue( const char * pszVariable )
{
	//	���ұ��ر�������
	char * p = m_xDefineVarList.GetVarValue( pszVariable );
	//	����ҵ������ؽ����
	if( p )return p;
	//	�Ҳ����Ļ���ֱ�ӷ��ػ���Ľ��
	return xVariableProvider::GetVariableValue( pszVariable );
}
//------------------------------------------------------------------------
VOID CScriptObjectMgr::LoadScript( const char * pszFileName )
{
	CScriptObject * pObject = new CScriptObject;
	if( pObject->Load( pszFileName ) )
	{
		m_xScriptList.Add( pObject->getName(), (LPVOID)pObject );
	}
	else
		delete pObject;
}
//------------------------------------------------------------------------
VOID CScriptObjectMgr::LoadDefine( const char * pszFileName )
{
	CStringFile sf( pszFileName );
	for( int i = 0;i < sf.GetLineCount();i ++ )
	{
		xStringsExtracter<3> define( sf[i], " \t", " \t\"" );
		if( define.getCount() == 2 || define.getCount() == 3 )
		{
			if( stricmp( define[0], "#define" ) == 0 )
			{
				char * pValue = NULL;
				if( (pValue = this->m_xDefineVarList.GetVarValue( define[1] )) != NULL )
				{
					PRINT( 0xff, "�����ض����� %s �ļ��� %u ��: %s �Ѿ������ֵ [%s]\n", pszFileName, i + 1, define[1], pValue );
				}
				else
				{
					m_xDefineVarList.AddVar( define[1], define[2] );
				}
			}
		}
	}
}
//------------------------------------------------------------------------