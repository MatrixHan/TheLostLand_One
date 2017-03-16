/**
*	created:		2013-4-19   9:24
*	filename: 		HumanPlayer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "AliveObject.h"
#include "ScriptTarget.h"
//------------------------------------------------------------------------
class CHumanPlayer :
	public CAliveObject
{
public:
	CHumanPlayer(){}
	virtual ~CHumanPlayer(){}
public:
	// �����ɫ��Ϣ��������
	bool	UpdateToDB(){ return true; };
	// �����ɫ��ȫ����Ϣ��������
	VOID	GetDBInfoPacket( xPacket & packet ){};
	// ��¼���ֲ���
	VOID	SaveVars(){};
	// ��ȡ���ֲ���
	VOID	LoadVars(){};
	// ��ȡ��ɫDB_Id
	DWORD	GetDBId(){  return m_dwDBId;}

	virtual CScriptTargetForPlayer* GetScriptTarget(){ return &m_ScriptTarget;}
public:
	// ��ȡ��ɫ����
	const char *	GetName(){  return ""; }
	// ��ȡ�ű���¼ֵ
	const char*		GetScriptVarValue( const char * pszName );
private:
	CScriptTargetForPlayer	m_ScriptTarget;
	UINT					m_dwDBId;
	char					m_szTempScriptVarValue[256];
};
//------------------------------------------------------------------------