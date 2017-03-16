/**
*	created:		2013-5-3   3:08
*	filename: 		CS_CommonDefine
*	author:			FreeKnight
*	Copyright (C): 	 
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// �û��˻�����󳤶ȡ�ʵ����󳤶�Ϊ15��
#define MAX_ACCOUT_LEN				16
// �û�������󳤶ȡ�ʵ����󳤶�Ϊ15��
#define MAX_PASSWORD_LEN			16
//------------------------------------------------------------------------
// һ���˺�������ɫ����
#define MAX_MAIN_ROLE_NUM			4
//------------------------------------------------------------------------
// ����������󳤶�[ char ]����\0
#define MAX_MAIN_ROLE_NAME_LEN		15		
//------------------------------------------------------------------------
// ������Ӫö��
enum EnumCountryCampType
{
	eCountryCampType_Unknow			= 0,
	eCountryCampType_DiSiKaiTe		= 1,	// ��˹����
	eCountryCampType_FeiEr			= 2,	// �Ѷ�		
	eCountryCampType_FeiErLong		= 3,	// �ƶ�¡
	eCountryCampType_KeManSaSi		= 4,	// ������˹
	eCountryCampType_NuoTaiEr		= 5,	// ŵ̩��
	eCountryCampType_Max,
};
//------------------------------------------------------------------------
// ��׼��Ӫö��
enum EnumCampType
{
	eCampType_Unknow				= 0,	// δ֪��Ӫ
	eCampType_LawfulGood			= 1,	// ��������
	eCampType_ChaoticGood			= 2,	// ��������
	eCampType_Neutral				= 3,	// ��������
	eCampType_ChaoticEvil			= 4,	// ����а��
	eCampType_LawfulEvil			= 5,	// ����а��
	eCampType_Max,						
};
//------------------------------------------------------------------------
// �Ա�ö��
enum EnumSexType
{
	eSexType_Unknow		= 0,
	eSexType_Male		= 1,		// ����
	eSexType_Female		= 2,		// Ů��
	eSexType_Max,
};
//------------------------------------------------------------------------
// ����ö��
enum EnumRaceType
{
	eRaceType_Unknow	= 0,
	eRaceType_Human		= 1,		// ����
	eRaceType_Elf		= 2,		// ����
	eRaceType_Dwarf		= 3,		// ����
	eRaceType_HalfOrc	= 4,		// ������
	eRaceType_Element	= 5,		// Ԫ�ؾ���
	eRaceType_Max,					

	// ���ﲢ����Щ��Ӫ���ѣ������������������
};
//------------------------------------------------------------------------
// ��ɫ��¼ʱ��Ϣ
struct stMainRoleLoginInfo
{
	char				m_szMainRoleName[MAX_MAIN_ROLE_NAME_LEN+1];	// ��ɫ����
	EnumCountryCampType	m_eCountryCamp;								// ����������Ӫ
	EnumSexType			m_eSex;										// �Ա�
	EnumRaceType		m_eRace;									// ����
	EnumCampType		m_eCamp;									// ��׼��Ӫ
	int					m_nLv;										// �ȼ�
	int					m_nHeroCardNum;								// Ӣ�ۿ�����

	stMainRoleLoginInfo()
	{
		memset( this, 0, sizeof(this) );
	}
};
//------------------------------------------------------------------------