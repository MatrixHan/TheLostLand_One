/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	LogConfig
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _LOG_CONFIG_H_
#define _LOG_CONFIG_H_
//--------------------------------------------------------------------
// Ĭ��Log�㼶
#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL FK_LOG_LEVEL_TRACE
#endif

#ifndef FK_LOG_LEVEL
#define FK_LOG_LEVEL LOG_DEFAULT_LEVEL
#endif
//--------------------------------------------------------------------
// Ĭ��Logģ������
#ifndef LOG_DEFAULT_MODULE_NAME
#define LOG_DEFAULT_MODULE_NAME "Unknown Module"
#endif
//--------------------------------------------------------------------
// Ĭ���ļ�����
#ifndef DEFAULT_FILE_LOG_NAME
#define DEFAULT_FILE_LOG_NAME "FKLog.txt"
#endif
//--------------------------------------------------------------------
// SocketLog������Ϣ��󳤶�
#ifndef MAX_SOCKET_BUF_SIZE
#define MAX_SOCKET_BUF_SIZE 1024
#endif
//--------------------------------------------------------------------
#endif