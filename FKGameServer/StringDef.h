/**
*	created:		2013-4-19   10:04
*	filename: 		StringDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
enum	string_define
{
	SD_EXECCMDERROR_BADCOMMAND,			//�������˴��������
	SD_EXECCMDERROR_GMEXECCMDLEVELLOW,	//����Ȩ�޲������޷�ִ�и�GM����
	SD_EXECCMDERROR_PLAYEREXECCMDLEVELLOW,//���ȼ��������޷�ʹ�ø�����
	SD_CMDRESULTVALUEIS,				//�����ֵ
	SD_CMDMAPERROR_SCRIPTNOTFOUND,		//����ӳ��ʧ�ܣ�û���ҵ�ҳ��
	SD_CMDMAPERROR_CMDNOTFOUND,			//����ӳ��ʧ�ܣ�û���ҵ��ڲ�����
	SD_CMDMAPERROR_CMDALREADYREGISTERED,//�����Ѿ���ע��
	SD_MAX,
};
//------------------------------------------------------------------------
static char * g_xStrings[SD_MAX] = {
	"�������˴��������",							//	SD_BAGCOMMAND,
	"����Ȩ�޲������޷�ִ�и�GM���",				//	SD_GMEXECCMDLEVELLOW
	"���ȼ��������޷�ʹ�ø����",					//	SD_PLAYEREXECCMDLEVELLOW,
	"�����ֵ: ",									//	SD_CMDRESULTVALUEIS,
	"����ӳ��ʧ�ܣ�û���ҵ�ҳ��",					//	SD_CMDMAPERROR_SCRIPTNOTFOUND
	"����ӳ��ʧ�ܣ�û���ҵ��ڲ�����",				//	SD_CMDMAPERROR_CMDNOTFOUND
	"�����Ѿ���ע��",								//	SD_CMDMAPERROR_CMDALREADYREGISTERED,
};
//------------------------------------------------------------------------
inline const char * getstrings( string_define sd )
{
	if( (UINT)sd >= (UINT)SD_MAX )return "<NULL>";
	return g_xStrings[sd];
}
//------------------------------------------------------------------------