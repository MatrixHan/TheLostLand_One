/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	FKSocket
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _FK_SOCKET_H_
#define _FK_SOCKET_H_
//--------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)
    #include <winsock2.h>
    typedef SOCKET FKSocket;
#elif defined(__unix) || defined(__linux)
    typedef int FKSocket;
#else
#endif
//--------------------------------------------------------------------
namespace FKLogger
{
    // ����һ��Socket������
    int FKCreateConnectedSocket( const char* p_pServerName, int p_nPort, FKSocket* p_pSockHandle );
    // ����Log���ݵ�Log������
    int FKSendLog( FKSocket p_SocketHandle, const char* p_pData, const int p_nDataSize );
	int	FKSendLog( FKSocket p_SocketHandle, const wchar_t* p_pData, const int p_nDataSize );
    // �ر�����һ��Socket����
    int FKDestroySocket( FKSocket* p_pSocket );
}
//--------------------------------------------------------------------
#endif