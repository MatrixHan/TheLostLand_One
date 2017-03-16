/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	LogLevel
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _LOG_LEVEL_H_
#define _LOG_LEVEL_H_

//--------------------------------------------------------------------

namespace FKLogger
{
    // ����Log��Ҫ�㼶
    #define FK_LOG_LEVEL_TRACE  100
    #define FK_LOG_LEVEL_DEBUG  200
    #define FK_LOG_LEVEL_INFO   300
    #define FK_LOG_LEVEL_WARN   400
    #define FK_LOG_LEVEL_ERROR  500
    #define FK_LOG_LEVEL_FATAL  600

    // ����ú���ʹ��ǰ�ļ�Log��Ч
    #define FK_LOG_LEVEL_DISABLED   1000

    typedef enum ENUM_LogLevel
    {
        ELL_Trace,
        ELL_Debug,
        ELL_Info,
        ELL_Warn,
        ELL_Error,
        ELL_Fatal,
    }ELogLevel;

    // ö��Log�������ʽ
    typedef enum ENUM_LogOutputMode
    {
        ELOM_File,
        ELOM_Console,
        ELOM_Socket,
    }ELogOutputMode;
}

//--------------------------------------------------------------------
#endif