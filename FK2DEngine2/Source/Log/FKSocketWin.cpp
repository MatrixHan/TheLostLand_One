/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	FKSocketWin
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#include <winsock2.h>
#include <winbase.h>
#include "../../Include/Log/LogConfig.h"
#include "../../Include/Log/FKSocket.h"
#include "../../Include/FKStringConvert.h"
#pragma comment( lib, "ws2_32.lib")

//--------------------------------------------------------------------
// ����һ��Socket������
int FKLogger::FKCreateConnectedSocket( const char* p_pServerName, int p_nPort, FKSocket* p_pSockHandle )
{
    if(( p_pServerName == NULL ) || ( p_pSockHandle == NULL ))
    {
        return -1;
    }

    struct sockaddr_in clientService;
    int nResult = 0;
    SOCKET sock = INVALID_SOCKET;

    // ��ʼ��WinSock
    WSADATA wsaData;
    nResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if( NO_ERROR != nResult )
    {
        goto WSA_EXIT_LABEL;
    }

    // ����һ��Socket
    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( INVALID_SOCKET == sock )
    {
        goto WSA_EXIT_LABEL;
    }

    clientService.sin_family        = AF_INET;
    clientService.sin_addr.s_addr   = inet_addr( p_pServerName );
    clientService.sin_port          = htons( p_nPort );

    // ���ӷ�����
    if( connect( sock, (SOCKADDR*)&clientService, sizeof(clientService) ) == SOCKET_ERROR )
    {
        goto WSA_EXIT_LABEL;
    }

    *p_pSockHandle = sock;
    return 0;

WSA_EXIT_LABEL:
    if( sock != INVALID_SOCKET )
    {
        closesocket( sock );
    }
    WSACleanup();
    return -2;
}
//--------------------------------------------------------------------
// ����Log���ݵ�Log������
int FKLogger::FKSendLog( FKSocket p_SocketHandle, const char* p_pData, const int p_nDataSize )
{
    return send( (SOCKET)p_SocketHandle, p_pData, p_nDataSize, 0 );
}
//--------------------------------------------------------------------
// ����Log���ݵ�Log������
int FKLogger::FKSendLog( FKSocket p_SocketHandle, const wchar_t* p_pData, const int p_nDataSize )
{
	static char szBuf[MAX_SOCKET_BUF_SIZE];
	FK2DEngine2::core::WcharToChar( p_pData, szBuf, MAX_SOCKET_BUF_SIZE - 1 );
	szBuf[MAX_SOCKET_BUF_SIZE - 1] = 0;
	return send( (SOCKET)p_SocketHandle, szBuf, strlen(szBuf), 0 );
}
//--------------------------------------------------------------------
// �ر�����һ��Socket����
int FKLogger::FKDestroySocket( FKSocket* p_pSocket )
{
    if( p_pSocket == NULL )
    {
        return -1;
    }
    closesocket( (SOCKET)*p_pSocket );
    *p_pSocket = INVALID_SOCKET;
    return 0;
}
//--------------------------------------------------------------------