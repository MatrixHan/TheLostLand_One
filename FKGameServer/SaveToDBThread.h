/**
*	created:		2013-4-18   23:46
*	filename: 		SaveToDBThread
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "SimpleDBConnection.h"
//------------------------------------------------------------------------
// ��ǰ�߳�״̬
enum	thread_state
{
	TS_CONNECTINGDBSERVER,		// ׼������DBSvr
	TS_WAITING,					// �ȴ�״̬
	TS_RUNNING,					// ������
};
//------------------------------------------------------------------------
class CHumanPlayer;
class CSaveToDBThread :
	public CSimpleDBConnection,
	public xThread,
	public xSingletonClass<CSaveToDBThread>
{
public:
	CSaveToDBThread(void);
	virtual ~CSaveToDBThread(void);
public:
	VOID OnMsg( MIRMSG * pMsg, int datasize );
protected:
	// ����ָ����ҵ�ȫ����Ϣ
	VOID DoWithPlayer( CHumanPlayer * pPlayer );
	// ��������������
	VOID Execute( LPVOID lpParam );
protected:
	CSimpleDBConnection * m_pxDBConnection;
	thread_state m_State;
};
//------------------------------------------------------------------------