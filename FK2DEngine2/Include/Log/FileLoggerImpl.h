/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	FileLoggerImpl
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _FILE_LOGGER_IMPL_H_
#define _FILE_LOGGER_IMPL_H_
//--------------------------------------------------------------------
#include "LogConfig.h"
#include "LogObject.h"
#include "FileLogger.h"
//--------------------------------------------------------------------
namespace FKLogger
{
    int InitFileLogger( FKLogger::SLogWriter** p_ppLogWriter, FKLogger::SFileLoggerInitParams* p_pParams );
    int InitConsoleLogger( FKLogger::SLogWriter** p_ppLogWriter, void* p_pInitParams );
}
//--------------------------------------------------------------------
#endif