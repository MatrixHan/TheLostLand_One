/**
*	created:		2013-5-10   7:39
*	filename: 		IFKAppNetworkLink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <winsock2.h>
#include "FKClientLink.h"
//------------------------------------------------------------------------
enum EnumNetworkLinkState
{
	eNLS_Unknow			= 0,
	eNLS_LoginSvr		= 1,	// ��½������
	eNLS_CharSelSvr		= 2,	// ��ɫѡ�������
	eNLS_GameSvr		= 3,	// ��Ϸ��������
	eNLS_Max,
};
//------------------------------------------------------------------------
struct IFKAppNetworkLink : public IClientLinkSink
{
	// �������ݰ�
	virtual int				Send(LPCSTR buf, int nLen) = NULL;

	// ȡ��IClientLinkָ��, ʹ�÷������ḻ
	virtual IClientLink *	GetClientLink(void) = NULL;

	// ���ص�ǰ���ӵ�����һ��״̬
	virtual int				GetType(void) = NULL;
};
//------------------------------------------------------------------------