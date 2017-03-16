/**
*	created:		2013-4-17   23:33
*	filename: 		CenterServerCodeMsgParser
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ClientObj.h"
#include "Server.h"
#include <map>
//------------------------------------------------------------------------
typedef WORD MSG_KEY;
using std::map;
//------------------------------------------------------------------------
typedef bool (*CodeMsgCallback)( CClientObj * pObject, PMIRMSG pMsg, int datasize );
//------------------------------------------------------------------------
class CCenterSvrMsgParser : public xSingletonClass< CCenterSvrMsgParser >
{
public:
	CCenterSvrMsgParser();
	~CCenterSvrMsgParser();
public:
	// ����������Ϣ
	bool	OnRecvCodeMsg( CClientObj * pObject, PMIRMSG pMsg, int datasize );
private:
	bool	__Init();
	void	__Release();
public:
	bool	RegisterMsgFunc( MSG_KEY, CodeMsgCallback );
	bool	UnRegisterMsgFunc( MSG_KEY );
private:
	typedef std::map< MSG_KEY, CodeMsgCallback >		MSG_FUNC_MAP;
	MSG_FUNC_MAP		m_mapMsgHandlers;
};
//------------------------------------------------------------------------
class CCodeMsgFunc
{
public:
	// ��ȡGS��ַ
	static bool OnMsg_GetGameSvrAddr( CClientObj * pObject, PMIRMSG pMsg, int datasize );
	// ע�������
	static bool OnMsg_RegisterSvr( CClientObj * pObject, PMIRMSG pMsg, int datasize );
	// ���·�����������
	static bool OnMsg_UpdateSvrInfo( CClientObj * pObject, PMIRMSG pMsg, int datasize );
	// ���ݷ����������ҷ�����IP
	static bool OnMsg_FindSvr( CClientObj * pObject, PMIRMSG pMsg, int datasize );
	// ��ȡGS�����б�
	static bool OnMsg_GSNameList( CClientObj * pObject, PMIRMSG pMsg, int datasize );


	// �����������Ϣ������Ϣ�����ķ������������ָ����������������Ϣת����
	static bool OnMsg_AcrossSvr( CClientObj * pObject, PMIRMSG pMsg, int datasize );
};
//------------------------------------------------------------------------