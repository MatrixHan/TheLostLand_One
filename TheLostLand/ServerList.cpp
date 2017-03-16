/**
*	created:		2013-5-3   17:55
*	filename: 		ServerList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "ServerList.h"
//------------------------------------------------------------------------
CServerList::~CServerList()
{
}
//------------------------------------------------------------------------
// ��ȡָ����Ϸ�ķ���������
int CServerList::GetServerNum( std::wstring& szGameName )
{
	SERVER_INFO_MAP::const_iterator Ite = m_ServerInfoMap.find( szGameName );
	if( Ite == m_ServerInfoMap.end() )
	{
		return 0;
	}
	return (int)( Ite->second.size() );
}
//------------------------------------------------------------------------
// ��ȡ����Ϸ���͵ĸ���
int CServerList::GetGameNum()
{
	return m_ServerInfoMap.size();
}
//------------------------------------------------------------------------
// ���ط������б�
bool CServerList::LoadFromFile( const char* filename )
{
	IResScript* pObj = g_pResMgr->LoadResScript( filename );
	if( pObj == NULL )
	{
		FKTRACE( L"��ȡ�ļ� %s ʧ�ܣ�", filename );
		return false;
	}
	int nNum = pObj->GetObjectCount();
	IResObject* pRoot = pObj->GetResObjectRoot();
	if( pRoot == NULL )
	{
		pObj->Release();
		return false;
	}

	int nSubNum = pRoot->GetSubObjectCount();
	bool bResult = true;
	for( int i = 0; i < nSubNum; ++i )
	{
		bResult &= Parser( pRoot->GetSubObjectByIndex(i), 0, "" );
	}
	if (!bResult) 
	{
		//��������
		FKTRACE( L"�����ļ� %s ����", filename );
		pObj->Release();
		return false;
	}
	// pObj->Dump();
	//pObj->Release();
	return true;
}
//------------------------------------------------------------------------
bool CServerList::Parser( IResObject *pResObject, int nParentID, std::string szGameName )
{
	if( pResObject == NULL )
	{
		return false;
	}
	const char* pAdd = pResObject->GetPropertyString( "Login������IP��ַ" );
	// ˵���Ƿ���������
	if( strcmp( pAdd,"��" ) == 0 )
	{
		bool bResult = true;
		int nSubCount = pResObject->GetSubObjectCount();
		for( int i = 0; i < nSubCount; ++i )
		{
			bResult &= Parser( pResObject->GetSubObjectByIndex(i), pResObject->GetID(), pResObject->GetObjectName() );
		}

		if (!bResult) 
		{
			//��������
			FKTRACE( L"�����ļ�����" );
			return false;
		}
	}
	else
	{// ˵�����ӷ�������
		SServerInfo tagInfo;
		tagInfo.m_nServerIndex = pResObject->GetID();
		tagInfo.m_szServerIP = pAdd;
		char szServerName[64];
		memset( szServerName, 0, 64 );
		memcpy( szServerName, pResObject->GetObjectName(), 64 - 1 );
		tagInfo.m_szServerName = FK2DEngine2::Utility::StringToUnicode( szServerName );
		
		std::wstring szTmpGameName;
		szTmpGameName = FK2DEngine2::Utility::StringToUnicode( szGameName );
		SERVER_INFO_MAP::iterator Ite = m_ServerInfoMap.find( szTmpGameName.c_str() );
		if( Ite != m_ServerInfoMap.end() )
		{
			// ������ԭ�����з�������
			Ite->second.push_back( tagInfo );
		}
		else
		{
			// ������ԭ��û�з�����
			SERVER_INFO_VEC vec;
			vec.push_back( tagInfo );
			m_ServerInfoMap.insert( std::make_pair( szTmpGameName, vec ) );
		}
	}
	return true;
}
//------------------------------------------------------------------------