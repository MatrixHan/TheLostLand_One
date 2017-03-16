/**
*	created:		2013-5-3   17:58
*	filename: 		DeveloperList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MainMacro.h"
//------------------------------------------------------------------------
enum ENUM_JobType
{
	eJobTypeCeHua		= 0,	// �߻�
	eJobTypeChengXu		= 1,	// ����
	eJobTypeMeiShu		= 2,	// ����
	eJobTypeYinYue		= 3,	// ����
	eJobTypeCeShi		= 4,	// ����
	eJobTypeYunYing		= 5,	// ��Ӫ

	eJobTypeMax,
};
//------------------------------------------------------------------------
struct SDeveloperInfo
{
	int						m_nIndex;		// �����߱��
	std::wstring			m_szJobName;	// ������ְλ
	std::wstring			m_szName;		// ����������
	std::wstring			m_szImageUrl;	// ������ͷ���ַ
	std::wstring			m_szInfo;		// ��������Ϣ���
};
//------------------------------------------------------------------------
class CDeveloperList : public TSingletonClass< CDeveloperList >
{
public:
	typedef std::vector< SDeveloperInfo >			DEVELOPER_INFO_VEC;
	typedef std::map< int, DEVELOPER_INFO_VEC >		DEVELOPER_INFO_MAP;
public:
	CDeveloperList(){};
	~CDeveloperList(){};
public:
	DEVELOPER_INFO_MAP				m_DeveloperInfoMap;
public:
	// ��ȡָ��ְҵ��Ŀ����߸���
	int								GetDeveloperNum( int nJobGroupID );
	// ���ط������б�
	bool							LoadFromFile( const char* filename );
};
//------------------------------------------------------------------------