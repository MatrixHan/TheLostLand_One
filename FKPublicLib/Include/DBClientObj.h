/**
*	created:		2013-4-16   5:07
*	filename: 		DBClientObj
*	author:			FreeKnight
*	Copyright (C): 	 FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "clientobject.h"
//------------------------------------------------------------------------
//��DBServerͨ��
class CDBClientObj : public CClientObject
{
public:
	CDBClientObj(void);
	virtual ~CDBClientObj(void);
	void Update();
public:
	xPacket * GetPacket(){ return m_xPacketQueue.pop();}
};
//------------------------------------------------------------------------