/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	SocketLogger
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _SOCKET_LOGGER_H_
#define _SOCKET_LOGGER_H_
//--------------------------------------------------------------------
namespace FKLogger
{
    typedef struct SSocketLoggerInitParams
    {
        char*       m_pServer;
        int         m_nPort;

        SSocketLoggerInitParams()
        {
            m_pServer = NULL;
            m_nPort = 0;
        }
    }socket_logger_init_params;
}
//--------------------------------------------------------------------
#endif