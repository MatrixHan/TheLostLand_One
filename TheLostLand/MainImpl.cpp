/**
*	created:		2013-5-3   20:32
*	filename: 		MainImpl
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "MainImpl.h"
#include "ServerList.h"
#include "DeveloperList.h"
//------------------------------------------------------------------------
// ���������ļ�
bool InitConfig()
{
	// ���ط������б�
	if( !CServerList::GetInstance()->LoadFromFile( SERVER_LIST_FILE ) )
	{
		FKTRACE( L"���ط������б�ʧ��." );
		return false;
	}

	// ���ؿ�����Ա�б�
	if( !CDeveloperList::GetInstance()->LoadFromFile( DEVELOPER_LIST_FILE ) )
	{
		FKTRACE( L"���ؿ�����Ա�б�ʧ��." );
		return false;
	}
	return true;
}
//------------------------------------------------------------------------