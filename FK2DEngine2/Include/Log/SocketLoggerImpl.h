/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	SocketLoggerImpl
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _SOCKET_LOGGER_IMPL_H_
#define _SOCKET_LOGGER_IMPL_H_
//--------------------------------------------------------------------
#include "LogObject.h"
#include "SocketLogger.h"
//--------------------------------------------------------------------
namespace FKLogger
{
    int InitSocketLogger( SLogWriter** p_ppLogWriter,SSocketLoggerInitParams *p_pParams );
}
//--------------------------------------------------------------------
#endif