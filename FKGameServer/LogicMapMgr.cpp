/**
*	created:		2013-4-19   14:33
*	filename: 		LogicMapMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\logicmapmgr.h"
#include "logicmap.h"
//------------------------------------------------------------------------
CLogicMapMgr * CLogicMapMgr::m_pInstance = NULL;
//------------------------------------------------------------------------
CLogicMapMgr::CLogicMapMgr(void):m_pLoadingMap(NULL)
{
	if( m_pInstance == NULL )
		m_pInstance = this;
	memset( m_pMaps, 0, sizeof( m_pMaps ) );
}
//------------------------------------------------------------------------
CLogicMapMgr::~CLogicMapMgr(void)
{
}
//------------------------------------------------------------------------
VOID CLogicMapMgr::OnFoundFile( const char * pszFilename, UINT nParam )
{
	if( m_pLoadingMap == NULL )
	{
		UINT id = m_LogicMaps.New( &m_pLoadingMap );
		if( id == 0 )
			m_pLoadingMap = NULL;
		else
			m_pLoadingMap->SetId( id );
	}

	assert( m_pLoadingMap != NULL );
	//	��ȡ��ͼ
	if( !m_pLoadingMap->LoadMap(pszFilename))
		return;
	//	��ӵ���ͼ���ֹ�����
	if( !m_MapNameHash.HAdd( m_pLoadingMap->GetTitle(), (LPVOID)m_pLoadingMap ) )
	{
		return;
	}
	UINT index = m_pLoadingMap->GetIndex();
	if( index >0 || index <= 2048 )
		m_pMaps[index-1] = m_pLoadingMap;
	//	�ɹ��󣬽�ָ�����óɿգ�����һ���ļ�����newһ������
	m_pLoadingMap = NULL;
}
//------------------------------------------------------------------------
VOID	CLogicMapMgr::Load( const char * pszPath )
{
	//	��ʼ�������е�ini�ļ�
	StartFind( pszPath, "*.ini" );
	//	������һ��ָ���ȡʧ�ܣ�ɾ�����һ��ָ�롣
	if( m_pLoadingMap != NULL )
		delete m_pLoadingMap;
	for( int i = 0;i < MAX_LOGIC_MAP; i++ )
	{
		if( m_pMaps[i] != NULL )
			m_pMaps[i]->InitLinks();
	}
}
//------------------------------------------------------------------------
CLogicMap * CLogicMapMgr::GetLogicMapByTitle( const char * pszMapTitle )
{
	return (CLogicMap*)m_MapNameHash.HGet(pszMapTitle);
}
//------------------------------------------------------------------------