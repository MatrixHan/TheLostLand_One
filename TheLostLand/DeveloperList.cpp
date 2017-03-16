/**
*	created:		2013-5-3   19:52
*	filename: 		DeveloperList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "DeveloperList.h"
//------------------------------------------------------------------------
// ��ȡָ��ְҵ��Ŀ����߸���
int CDeveloperList::GetDeveloperNum( int nJobGroupID )
{
	if( nJobGroupID >= eJobTypeMax || nJobGroupID < 0 )
	{
		return 0;
	}
	DEVELOPER_INFO_MAP::iterator Ite = m_DeveloperInfoMap.find( nJobGroupID );
	if( Ite != m_DeveloperInfoMap.end() )
	{
		return ( Ite->second.size() );
	}
	return 0;
}
//------------------------------------------------------------------------
// ���ط������б�
bool CDeveloperList::LoadFromFile( const char* filename )
{
	IResScript* pObj = g_pResMgr->LoadResScript( filename );
	if( pObj == NULL )
	{
		FKTRACE( L"��ȡ�ļ� %s ʧ�ܣ�", filename );
		return false;
	}
	// ������Ա�������
	int nNum = pObj->GetObjectCount();
	IResObject* pRoot = pObj->GetResObjectRoot();
	if( pRoot == NULL )
	{
		pObj->Release();
		return false;
	}

	// ��ȡ������
	int nSubNum = pRoot->GetSubObjectCount();
	bool bResult = true;
	IResObject* pCur = NULL;
	int nMainType = eJobTypeMax;
	char szTemp[512];
	for( int i = 0; i < nSubNum; ++i )
	{
		pCur = pRoot->GetSubObjectByIndex(i);
		if( pCur == NULL )
		{
			bResult = false;
			break;
		}
		const char* pMainTile = pCur->GetObjectName();
		if( strcmp( pMainTile, "�߻�") == 0 )
		{
			nMainType = eJobTypeCeHua;
		}
		else if( strcmp( pMainTile, "����") == 0 )
		{
			nMainType = eJobTypeChengXu;
		}
		else if( strcmp( pMainTile, "����") == 0 )
		{
			nMainType = eJobTypeMeiShu;
		}
		else if( strcmp( pMainTile, "����") == 0 )
		{
			nMainType = eJobTypeYinYue;
		}
		else if( strcmp( pMainTile, "����") == 0 )
		{
			nMainType = eJobTypeCeShi;
		}
		else if( strcmp( pMainTile, "��Ӫ") == 0 )
		{
			nMainType = eJobTypeYunYing;
		}

		SDeveloperInfo info;
		info.m_nIndex = pCur->GetID();
		memset( szTemp, 0, 512 );
		strcpy_s( szTemp, pCur->GetPropertyString("��ְλ") );
		info.m_szJobName = FK2DEngine2::Utility::StringToUnicode( szTemp );
		strcpy_s( szTemp, pCur->GetPropertyString("����") );
		info.m_szName = FK2DEngine2::Utility::StringToUnicode( szTemp );
		strcpy_s( szTemp, pCur->GetPropertyString("ͷ��") );
		info.m_szImageUrl = FK2DEngine2::Utility::StringToUnicode( szTemp );
		strcpy_s( szTemp, pCur->GetPropertyString("����") );
		info.m_szInfo = FK2DEngine2::Utility::StringToUnicode( szTemp );

		DEVELOPER_INFO_MAP::iterator Ite = m_DeveloperInfoMap.find( nMainType );
		if( Ite != m_DeveloperInfoMap.end() )
		{
			// ������ԭ��������
			Ite->second.push_back( info );
		}
		else
		{
			// ������ԭ��û��
			DEVELOPER_INFO_VEC vec;
			vec.push_back( info );
			m_DeveloperInfoMap.insert( std::make_pair( nMainType, vec ) );
		}
	}
	if (!bResult) 
	{
		//��������
		FKTRACE( L"�����ļ� %s ����", filename );
		pObj->Release();
		return false;
	}

	// pObj->Release();
	return true;
}
//------------------------------------------------------------------------