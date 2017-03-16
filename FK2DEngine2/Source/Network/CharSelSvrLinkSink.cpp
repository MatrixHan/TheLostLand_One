/**
*	created:		2013-5-10   8:23
*	filename: 		CharSelSvrLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/Network/CharSelSvrLinkSink.h"
#include "../../Include/FKLogger.h"
//------------------------------------------------------------------------
CCharSelSvrLink g_CharSelSvrConnector;
//------------------------------------------------------------------------
CCharSelSvrLink::CCharSelSvrLink(void)
{
	m_pClientLink = NULL;
}
//------------------------------------------------------------------------
// ��  �������ӷ�����ʧ��
// ��  ����szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
void CCharSelSvrLink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	wchar_t szTemp[256];
	wsprintf(szTemp, L"����CharSel������ʧ�ܡ�[SockID = %d�� Msg��%s��ErrorCode��%d]", dwSocketID, szMsg, dwErrorCode);
	LogError( szTemp );
}
//------------------------------------------------------------------------
// ��  �������ӽ���
void CCharSelSvrLink::OnConnectOK(DWORD dwSocketID)
{	
	LogInfo( L"����CharSel�������ɹ��� %d", dwSocketID );
}
//------------------------------------------------------------------------
// ��  ������ǰ���ӶϿ�
// ��  ����szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
void CCharSelSvrLink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	wchar_t szTemp[256];
	wsprintf(szTemp, L"CharSel���������ӶϿ���[SockID = %d�� Msg��%s��ErrorCode��%d]", dwSocketID, szMsg, dwErrorCode);
	LogInfo( szTemp );
}
//------------------------------------------------------------------------
// ��  �����������ݰ�
int	CCharSelSvrLink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
// ��  ����ȡ��IClientLinkָ��, ʹ�÷������ḻ
IClientLink * CCharSelSvrLink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
// ��  �����յ�һ�����ݰ�
// ��  ��������������ݰ����߳���IThreadRoute����ͬһ�̣߳�
// ����ֵ������һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
BOOL CCharSelSvrLink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
{
	if(nLen < sizeof(WORD))
	{
		return FALSE;
	}

	WORD wMsgRoot = *((WORD *)(buf));
	switch(wMsgRoot)
	{
	default:
		{
			LogWarn( L"CCharSelSvrLink::OnRecv�յ�������Ϣ %d", wMsgRoot);
		}
		break;
	}

	return TRUE;
}
//------------------------------------------------------------------------
int CCharSelSvrLink::GetType(void)
{
	return eNLS_CharSelSvr;
}
//------------------------------------------------------------------------