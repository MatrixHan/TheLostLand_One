/**
*	created:		2013-5-10   2:19
*	filename: 		FKAPIProxy
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		���ض�WinAPI�ĵ���
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// dbghelp �ڵĺ�
#define _IMAGEHLP_SOURCE_
//------------------------------------------------------------------------
#include <winsock2.h>
#include <dbghelp.h>
//------------------------------------------------------------------------
#define ASSERT(p)			if( !(p) ){ __asm{ int 3 } };
//------------------------------------------------------------------------
// ȡ��һ��API�����ĵ�ַ
#define LOAD_API_FUNCTION(APINAME,APIDLL,name1,name2,name3)	\
	P##APINAME	API##APINAME()								\
	{														\
	static P##APINAME	s_pAPIFuntion = NULL;				\
	if( s_pAPIFuntion )										\
	return s_pAPIFuntion;								\
	HINSTANCE hInstance = GetModuleHandle(APIDLL);			\
	if( hInstance == NULL )									\
	hInstance = LoadLibrary(APIDLL);					\
	ASSERT(hInstance);										\
	char szName[256]={0};									\
	sprintf( szName, "%s%s%s",name1,name2,name3);			\
	s_pAPIFuntion = 										\
	(P##APINAME)GetProcAddress(hInstance,/*#APINAME*/szName);	\
	ASSERT(s_pAPIFuntion);									\
	if( IsAPIHooked(s_pAPIFuntion) )						\
		BlowUp();										    \
	return s_pAPIFuntion;									\
	}														\
	
//------------------------------------------------------------------------
// �������±�ķ�ʽ����
#define LOAD_API_BYFUNINDEX(APINAME,APIDLL,index)			\
	P##APINAME	API##APINAME()								\
	{														\
	static P##APINAME	s_pAPIFuntion = NULL;				\
	if( s_pAPIFuntion )										\
	return s_pAPIFuntion;									\
	HINSTANCE hInstance = GetModuleHandle(APIDLL);			\
	if( hInstance == NULL )									\
	hInstance = LoadLibrary(APIDLL);						\
	ASSERT(hInstance);										\
	s_pAPIFuntion = 										\
	(P##APINAME)GetProcAddress(hInstance,(LPCSTR)index);	\
	ASSERT(s_pAPIFuntion);									\
	return s_pAPIFuntion;									\
	}														\

//------------------------------------------------------------------------
#define DECLARE_API_FUNCTION(APINAME)						\
	P##APINAME API##APINAME();								\

//------------------------------------------------------------------------
// ���庯��ָ������
typedef BOOL (IMAGEAPI *PStackWalk)(  DWORD MachineType, 
									  HANDLE hProcess, 
									  HANDLE hThread, 
									  LPSTACKFRAME StackFrame, 
									  PVOID ContextRecord, 
									  PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,  
									  PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
									  PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, 
									  PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );

typedef int (WSAAPI *Psend)(		 SOCKET s,
									 const char FAR * buf,
									 int len,
									 int flags
									 );

typedef int (WSAAPI *Precv)(		SOCKET s,
									 char FAR * buf,
									 int len,
									 int flags
									 );
typedef int (WSAAPI *PWSAConnect)(
								   	SOCKET s,
								   	const struct sockaddr FAR * name,
								   	int namelen,
								   	LPWSABUF lpCallerData,
								   	LPWSABUF lpCalleeData,
								   	LPQOS lpSQOS,
								   	LPQOS lpGQOS
								   	);
//------------------------------------------------------------------------
DECLARE_API_FUNCTION(StackWalk)
DECLARE_API_FUNCTION(send)
DECLARE_API_FUNCTION(recv)
DECLARE_API_FUNCTION(WSAConnect)
//------------------------------------------------------------------------
#define StackWalk	APIStackWalk()
#define send		APIsend()
#define recv		APIrecv()
#define WSAConnect	APIWSAConnect()
//------------------------------------------------------------------------
// һ��API�Ƿ񱻹ҽ�
BOOL IsAPIHooked( void * pFun );
// ���ֱ�HOOK�ͱ���
void BlowUp();
// ����Ƿ���98ϵͳ
BOOL IsWindows98();
//------------------------------------------------------------------------