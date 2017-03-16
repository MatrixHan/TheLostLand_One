/**
*	created:		2013-5-10   8:03
*	filename: 		LoginSvrLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/Network/LoginSvrLinkSink.h"
#include "../../Include/FKLogger.h"
//------------------------------------------------------------------------
CLoginSvrLink g_LoginSvrConnector;
//------------------------------------------------------------------------
CLoginSvrLink::CLoginSvrLink(void)
{
	m_pClientLink = NULL;
}
//------------------------------------------------------------------------
// ��  �������ӷ�����ʧ��
// ��  ����szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
void CLoginSvrLink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	wchar_t szTemp[256];
	wsprintf(szTemp, L"����Login������ʧ�ܡ�[SockID = %d�� Msg��%s��ErrorCode��%d]", dwSocketID, szMsg, dwErrorCode);
	LogError( szTemp );
}
//------------------------------------------------------------------------
// ��  �������ӽ���
void CLoginSvrLink::OnConnectOK(DWORD dwSocketID)
{	
	LogInfo( L"����Login�������ɹ��� %d", dwSocketID );
}
//------------------------------------------------------------------------
// ��  ������ǰ���ӶϿ�
// ��  ����szMsgΪ������Ϣ�������ݡ�dwErrorCodeΪsocket������룬�μ�Win32 API : GetLastError()
void CLoginSvrLink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	wchar_t szTemp[256];
	wsprintf(szTemp, L"Login���������ӶϿ���[SockID = %d�� Msg��%s��ErrorCode��%d]", dwSocketID, szMsg, dwErrorCode);
	LogInfo( szTemp );
}
//------------------------------------------------------------------------
// ��  �����������ݰ�
int	CLoginSvrLink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
// ��  ����ȡ��IClientLinkָ��, ʹ�÷������ḻ
IClientLink * CLoginSvrLink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
// ��  �����յ�һ�����ݰ�
// ��  ��������������ݰ����߳���IThreadRoute����ͬһ�̣߳�
// ����ֵ������һ������(ILinkWnd)�������ͨѶ����ILinkWnd���븺����һ���������ݰ�����
BOOL CLoginSvrLink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
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
			LogWarn( L"CLoginSvrLink::OnRecv�յ�������Ϣ %d", wMsgRoot);
		}
		break;
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
int CLoginSvrLink::GetType(void)
{
	return eNLS_LoginSvr;
}
//------------------------------------------------------------------------