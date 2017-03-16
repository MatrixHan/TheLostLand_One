/**
*	created:		2013-5-10   10:36
*	filename: 		GameSvrLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <winsock2.h>
#include "../../Include/Network/GameSvrLinkSink.h"
#include "../../Include/FKLogger.h"
#include <algorithm>
//------------------------------------------------------------------------
CGameSvrLink g_GameSvrConnector;
//------------------------------------------------------------------------
LPCSTR  szRunErrDesc = "�������(GameSvr)���ӶϿ�, �����µ�¼��Ϸ! ������ٶȳ���,���ʼ���ϵ duzhi5368@163.com";
//------------------------------------------------------------------------
CGameSvrLink::CGameSvrLink(void)
{
	m_bThreadRun = FALSE;
	m_pClientLink = NULL;
	m_hEventWakeDecode = NULL;
	
	m_Thread.Close();
	if(!m_Thread.Create(NULL))
	{
		return;
	}

	DWORD dwEventID;
	if(!m_Thread->AddEvent(static_cast<IEventSink*>(this),dwEventID))
		return;

	m_hEventWakeDecode = m_Thread->GetEventHandle(dwEventID);

	if(!m_Thread->Start())
	{
		m_Thread.Close();
		return;
	}

	return;
}
//------------------------------------------------------------------------
CGameSvrLink::~CGameSvrLink(void)
{
	m_Thread.Close();
}
//------------------------------------------------------------------------
// ��  �������ӷ�����ʧ��
// ��  ����szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
void CGameSvrLink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	wchar_t szTemp[256];
	wsprintf(szTemp, L"����Game������ʧ�ܡ�[SockID = %d�� Msg��%s��ErrorCode��%d]", dwSocketID, szMsg, dwErrorCode);
	LogError( szTemp );
}
//------------------------------------------------------------------------
// ��  �������ӽ���
void CGameSvrLink::OnConnectOK(DWORD dwSocketID)
{
	LogInfo( L"����Game�������ɹ��� %d", dwSocketID );
}
//------------------------------------------------------------------------
// ��  ������ǰ���ӶϿ�
// ��  ����szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
void CGameSvrLink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	wchar_t szTemp[256];
	wsprintf(szTemp, L"Game���������ӶϿ���[SockID = %d�� Msg��%s��ErrorCode��%d]", dwSocketID, szMsg, dwErrorCode);
	LogInfo( szTemp );
}
//------------------------------------------------------------------------
// ��  �����������ݰ�
int	CGameSvrLink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
// ��  ����ȡ��IClientLinkָ��, ʹ�÷������ḻ
IClientLink * CGameSvrLink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
//��Ҫ�������ȡ���ݵ�ģ�� , ͨ���˺���ע���,�ſ��յ���Ϣ 
BOOL CGameSvrLink::AddReceiver(IDataReceiver *pReceiver, int nMsgCode)
{
	MESSAGE_LIST  *pList  = &m_MsgList;
	int           MsgCode = nMsgCode;

	//�ڱ��������ע����
	RECEIVER_LIST::iterator itor = find((*pList)[MsgCode].begin(), 
		(*pList)[MsgCode].end(), pReceiver);
	
	if (itor != (*pList)[MsgCode].end())
	{//�������Ѿ�ע����
		return FALSE;
	}

	(*pList)[MsgCode].push_back(pReceiver);
	return TRUE;
}
//------------------------------------------------------------------------
//�Ƴ�ģ�������
BOOL CGameSvrLink::RemoveReceiver(IDataReceiver *pReceiver, int nMsgCode) 
{
	MESSAGE_LIST  *pList  = &m_MsgList;
	int           MsgCode = nMsgCode;

	(*pList)[MsgCode].remove(pReceiver);
	return TRUE;
}
//------------------------------------------------------------------------
// ��  �����յ�һ�����ݰ�
// ��  ��������������ݰ����߳���IThreadRoute����ͬһ�̣߳�
// ����ֵ������һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
BOOL CGameSvrLink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
{
	if(nLen < sizeof(WORD))
	{
		return FALSE;
	}
	
	stReceiverInfoNode node;
	node.pData = new char[nLen];
	memcpy(node.pData, buf, nLen);
	node.nLen = nLen;

	m_NodeList.push_back(node);
	
	if (!m_bThreadRun)
	{
		while(m_NodeList.size() != 0)
			Receiver();
		return TRUE;
	}

	SetEvent(m_hEventWakeDecode);

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CGameSvrLink::Receiver()
{
	int nCount = m_NodeList.size();
	if(nCount == 0)
	{
		return FALSE;
	}

	stReceiverInfoNode node = m_NodeList.front();
	m_NodeList.pop_front();
	LPCSTR buf = node.pData;
	
	WORD wMsgRoot = *((WORD *)(buf));
	int hsize = sizeof(WORD);         //��Ϣ��ĳ���

	switch(wMsgRoot)
	{
	default:
		{
			for (RECEIVER_LIST::iterator itor = m_MsgList[wMsgRoot].begin();
			itor != m_MsgList[wMsgRoot].end(); ++itor)
			{
				(*itor)->OnReceived(buf + hsize, node.nLen - hsize);
			} 
		}
		break;
	}

	delete[] node.pData;
	
	return TRUE;
}
//------------------------------------------------------------------------
void CGameSvrLink::OnEvent(DWORD dwEventID)
{
	m_Lock.Lock();

	if(Receiver())
		SetEvent(m_hEventWakeDecode);

	m_Lock.Unlock();

	return;
}
//------------------------------------------------------------------------
void CGameSvrLink::LockReceiver(BOOL bLock)
{
	if(bLock)
	{
		m_Lock.Lock();		
		if (!m_bThreadRun)
		{
			while(m_NodeList.size() != 0)
				Receiver();
		}
	}
	else
		m_Lock.Unlock();
	
}
//------------------------------------------------------------------------
void CGameSvrLink::SetThreadRun(BOOL bRun)
{
	m_bThreadRun = bRun;
}
//------------------------------------------------------------------------
int CGameSvrLink::GetType(void)
{
	return eNLS_GameSvr;
}
//------------------------------------------------------------------------