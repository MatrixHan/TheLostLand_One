/**
*	created:		2013-4-15   22:41
*	filename: 		ClientObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
typedef struct tagLOGINENTER
{
	tagLOGINENTER()
	{
		memset( this, 0, sizeof( *this));
	}
	CHAR	szAccount[12];
	UINT	nLid;
	UINT	nSid;
	DWORD	dwEnterTime;
	UINT	nListId;
}LOGINENTER;
//------------------------------------------------------------------------
class CClientObj :
	public CClientObject
{
public:
	CClientObj(void);
	virtual ~CClientObj(void);
public:
	VOID	Clean();
	VOID	Update();
	// δ������Ϣ
	VOID	OnUnCodeMsg( xClientObject * pObject, char * pszMsg, int size );
	// ������Ϣ
	VOID	OnCodedMsg( xClientObject * pObject, PMIRMSG pMsg, int datasize );
	// DBSvr��Ϣ
	VOID	OnDBMsg( PMIRMSG pMsg, int datasize );
	// CenterSvr��Ϣ
	VOID	OnSCMsg( PMIRMSG pMsg, int datasize );
	// GameSvr��Ϣ
	VOID	OnMASMsg( WORD wCmd, WORD wType, WORD wIndex, const char * pszData, int datasize );
	VOID	OnConnection();
protected:
	VOID	OnUnknownMsg( PMIRMSG pMsg, int datasize );
protected:
	UINT				m_nLoginId;
	UINT				m_nSelectId;
	LOGINENTER			m_EnterInfo;
	selcharserverstate	m_State;			// �ͻ��˽�ɫ��ǰ״̬
	FINDSERVER_RESULT	m_GameServerAddr;
	UINT				m_nFailCount;
	CServerTimer		m_TimeOut;
	BOOL				m_bSelected;
	CHAR				m_szCharName[32];	// ��ɫ����
	BOOL				m_bWaitForVerify;
	CServerTimer		m_WaitForVerifyTimer;
};
//------------------------------------------------------------------------