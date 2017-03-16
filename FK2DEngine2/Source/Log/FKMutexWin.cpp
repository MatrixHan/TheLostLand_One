/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	FKMutexWin
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#include <windows.h>
#include "../../Include/Log/FKMutex.h"

//--------------------------------------------------------------------
// �����߳���
int FKLogger::FKCreateMutex( FKMutex* p_pMutex )
{
    if( p_pMutex == NULL )
    {
        return -1;
    }

    HANDLE hMutex = CreateMutex( NULL, FALSE, NULL );

    if( hMutex == INVALID_HANDLE_VALUE )
    {
        return -2;
    }

    *p_pMutex = hMutex;
    return 0;
}
//--------------------------------------------------------------------
// ����
int FKLogger::FKLockMutex( FKMutex p_Mutex )
{
    if( p_Mutex == INVALID_HANDLE_VALUE )
    {
        return -1;
    }

    if( WaitForSingleObject( p_Mutex, INFINITE ) != WAIT_OBJECT_0 )
    {
        return -2;
    }
    return 0;
}
//--------------------------------------------------------------------
// ����
int FKLogger::FKUnlockMutex( FKMutex p_Mutex )
{
    if( p_Mutex == INVALID_HANDLE_VALUE )
    {
        return -1;
    }

    if( ReleaseMutex( p_Mutex ) == 0 )
    {
        return -2;
    }

    return 0;
}
//--------------------------------------------------------------------
// �����߳���
int FKLogger::FKDestroyMutex( FKMutex* p_pMutex )
{
    if( (p_pMutex == NULL) || ((*p_pMutex) == INVALID_HANDLE_VALUE) )
    {
        return -1;
    }

    if( CloseHandle( (HANDLE)*p_pMutex ) != 0 )
    {
        return -2;
    }
    
    *p_pMutex = 0;
    return 0;
}
//--------------------------------------------------------------------