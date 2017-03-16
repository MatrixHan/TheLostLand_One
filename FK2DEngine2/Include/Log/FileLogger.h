/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	FileLogger
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _FILE_LOGGER_H_
#define _FILE_LOGGER_H_

//--------------------------------------------------------------------
namespace FKLogger
{
    // �ļ�������
    typedef enum ENUM_FileOpenMode
    {
        EFOM_ClearMode = 0,
        EFOM_AppendMode
    }EFileOpenMode;

    // �ļ�Log��ʼ������
    typedef struct SFileLoggerInitParams
    {
        // �ļ�Log���ļ���
        char*               m_pFileName;
        ENUM_FileOpenMode   m_eMode;

        SFileLoggerInitParams()
        {
            m_pFileName = 0;
            m_eMode = EFOM_ClearMode;
        }
    }file_logger_init_params;
}
//--------------------------------------------------------------------
#endif