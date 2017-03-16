/********************************************************************
*
*	Copyright(c) FreeKnightDuzhi 2011
*   License�� LGPL
*   E-Mail��duzhi5368@163.com
* 	
*	-----------------------------------------------------------------
*
*	�ļ�����	FKMutex
*	���ߣ�		FreeKnightDuzhi[ ������ ]
*	�ļ�˵����	
*	����ʱ�䣺	2011/08/02
*
*	�޸���:		
*	�޸�����:
*********************************************************************/

#ifndef _FK_MUTEX_H_
#define _FK_MUTEX_H_
//--------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32)
    #include <windows.h>
    typedef HANDLE FKMutex;
#elif defined(__unix) || defined(__linux)
    #include <pthread.h>
    typedef pthread_mutex_t* FKMutex;
#else
#endif
//--------------------------------------------------------------------
namespace FKLogger
{
    // �����߳���
    int FKCreateMutex( FKMutex* p_pMutex );
    // ����
    int FKLockMutex( FKMutex p_Mutex );
    // ����
    int FKUnlockMutex( FKMutex p_Mutex );
    // �����߳���
    int FKDestroyMutex( FKMutex* p_pMutex );
}
//--------------------------------------------------------------------
#endif