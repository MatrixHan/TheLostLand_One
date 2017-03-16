/**
*	created:		2013-4-15   22:01
*	filename: 		ClientObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
typedef struct tagREGISTEREDSERVER
{
	char		szName[64];			// ���ӵķ���������
	ServerId	Ident;				// ��������ʾ��ϢID	
	SERVERADDR	Addr;				// ������IP��Ϣ
	int			iSendDBCount;		
	DWORD		dwConnections;
}REGISTEREDSERVER;
//------------------------------------------------------------------------
class CClientObj : public CClientObject
{
public:
	CClientObj(void);
	virtual ~CClientObj(void);
public:
	// �����ڲ�����
	VOID	Clean();
	// ÿ֡����
	VOID	Update();
	// ����δ������Ϣ
	VOID	OnUnCodeMsg( xClientObject * pObject, char * pszMsg, int size );
	// ����������Ϣ
	VOID	OnCodedMsg( xClientObject * pObject, PMIRMSG	pMsg, int datasize );
	REGISTEREDSERVER * GetRegInfo(){ return &m_RegInfo;}
public:
	REGISTEREDSERVER m_RegInfo;
};
//------------------------------------------------------------------------