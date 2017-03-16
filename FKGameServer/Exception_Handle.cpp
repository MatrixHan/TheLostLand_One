/**
*	created:		2013-4-18   23:29
*	filename: 		Exception_Handle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "stdafx.h"
#include "pe_debug.h"
#include "savetodbthread.h"
//------------------------------------------------------------------------
void StackDump(  FILE *fp, DWORD EBP, DWORD EIP )
{

	// The structure of the stack frames is the following:
	// EBP -> parent stack frame EBP
	//        return address for this call ( = caller )
	// The chain can be navigated iteratively; the
	// initial value of EBP is given by the parameter.
	// If EIP != 0, then before we dump the call stack, we
	// dump the same information for EIP. Basically, we
	// consider EIP as the first element of the call stack.
	int iMaxLoopCount = 60;
	static PE_Debug PE_debug ;
	BOOL EBP_OK = TRUE ;
	do
	{
		// Check if EBP is a good address
		// I'm expecting a standard stack frame, so
		// EPB must be aligned on a DWORD address
		// and it should be possible to read 8 bytes
		// starting at it (next EBP, caller).
		if( (DWORD)EBP & 3 )
			EBP_OK = FALSE ;
		if( EBP_OK && IsBadReadPtr( (void*)EBP, 8 ) )
			EBP_OK = FALSE ;
		if( EBP_OK )
		{
			BYTE* caller = EIP ? (BYTE*)EIP : *((BYTE**)EBP + 1) ;
			EBP = EIP ? EBP : *(DWORD*)EBP ;
			if( EIP )
				EIP = 0 ; // So it is considered just once
			// Get the instance handle of the module where caller belongs to
			MEMORY_BASIC_INFORMATION mbi ;
			VirtualQuery( caller, &mbi, sizeof( mbi ) ) ;
			// The instance handle is equal to the allocation base in Win32
			HINSTANCE hInstance = (HINSTANCE)mbi.AllocationBase ;
			// If EBP is valid, hInstance is not 0
			if( hInstance == 0 )
				EBP_OK = FALSE ;
			else
				PE_debug.DumpDebugInfo( fp, caller, hInstance ) ;

		}
		else
			break ; // End of the call chain

		iMaxLoopCount --;
	}
	while( TRUE && iMaxLoopCount>0) ;
	PE_debug.ClearReport() ;  // Prepare for future calls
}
//------------------------------------------------------------------------
void DumpExceptCallsStack( FILE *fp,  DWORD EBP, DWORD EIP )
{

	const char* separator = "------------------------------------------------------------------\r\n" ;
	fprintf( fp,  "\r\n���ö�ջ:\r\n" ) ;
	fprintf( fp,  separator ) ;
	StackDump( fp, EBP, EIP ) ;
	fprintf( fp,  separator ) ;
}
//------------------------------------------------------------------------
struct ExceptionCodeDescr
{
	DWORD code ;
	const char* descr ;
} ;
//------------------------------------------------------------------------
#define CODE_DESCR( c ) { c, #c }
//------------------------------------------------------------------------
static ExceptionCodeDescr exceptionTable[] =
{   
	CODE_DESCR( EXCEPTION_ACCESS_VIOLATION ),
		CODE_DESCR( EXCEPTION_ARRAY_BOUNDS_EXCEEDED ),
		CODE_DESCR( EXCEPTION_BREAKPOINT ),
		CODE_DESCR( EXCEPTION_DATATYPE_MISALIGNMENT ),
		CODE_DESCR( EXCEPTION_FLT_DENORMAL_OPERAND ),
		CODE_DESCR( EXCEPTION_FLT_DIVIDE_BY_ZERO ),
		CODE_DESCR( EXCEPTION_FLT_INEXACT_RESULT ),
		CODE_DESCR( EXCEPTION_FLT_INVALID_OPERATION ),
		CODE_DESCR( EXCEPTION_FLT_OVERFLOW ),
		CODE_DESCR( EXCEPTION_FLT_STACK_CHECK ),
		CODE_DESCR( EXCEPTION_FLT_UNDERFLOW ),
		CODE_DESCR( EXCEPTION_GUARD_PAGE ),
		CODE_DESCR( EXCEPTION_ILLEGAL_INSTRUCTION ),
		CODE_DESCR( EXCEPTION_IN_PAGE_ERROR ),
		CODE_DESCR( EXCEPTION_INT_DIVIDE_BY_ZERO ),
		CODE_DESCR( EXCEPTION_INT_OVERFLOW ),
		CODE_DESCR( EXCEPTION_INVALID_DISPOSITION ),
		CODE_DESCR( EXCEPTION_NONCONTINUABLE_EXCEPTION ),
		CODE_DESCR( EXCEPTION_PRIV_INSTRUCTION ),
		CODE_DESCR( EXCEPTION_SINGLE_STEP ),
		CODE_DESCR( EXCEPTION_STACK_OVERFLOW )
		// might be extended with compiler-specific knowledge of
		// "C++ Exception" code[s]
} ;
//------------------------------------------------------------------------
void DumpExceptionDescr( FILE *fp, DWORD code )
{
	const char* descr = NULL ;
	int n = sizeof( exceptionTable ) / sizeof( ExceptionCodeDescr ) ;
	for( int i = 0; i < n; i++ )
		if( exceptionTable[ i ].code == code )
		{
			descr = exceptionTable[ i ].descr ;
			break ;
		}
		if( descr )
			fprintf( fp, "  ����: %s\r\n", descr ) ;
		else
			fprintf( fp,  "  û������\r\n" ) ;
}
//------------------------------------------------------------------------
LONG WINAPI StackTraceExceptionFilter( LPEXCEPTION_POINTERS e )
{
	if( e )
	{
		LPEXCEPTION_POINTERS pException_ptr = e;
		FILE * fp = fopen( "exception_.log", "a+" );
		if( fp )
		{
			SYSTEMTIME	st;
			GetLocalTime( &st );
			fprintf( fp, "[%04u-%02u-%02u %02u:%02u:%02u.%03u]����������������\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );

			if( pException_ptr == NULL )
				fprintf( fp, "û����Ϣ����\n" );
			else
			{
				if( pException_ptr->ExceptionRecord == NULL )
					fprintf( fp, "û���쳣��¼��\n" );
				else
				{
					PEXCEPTION_RECORD pRecord = pException_ptr->ExceptionRecord;
					while( pRecord )
					{
						fprintf( fp, "�����: 0x%08x\n", pException_ptr->ExceptionRecord->ExceptionCode );
						fprintf( fp, "��ַ: 0x%08x\n", pException_ptr->ExceptionRecord->ExceptionAddress );
						if( EXCEPTION_ACCESS_VIOLATION == pException_ptr->ExceptionRecord->ExceptionCode )
							fprintf( fp, "����%s�ڴ�[0x%08x]ʱ��������\n", pException_ptr->ExceptionRecord->ExceptionInformation[0] == 0 ? "��ȡ": "д��", pException_ptr->ExceptionRecord->ExceptionInformation[1] );
						pRecord = pRecord->ExceptionRecord;
					}
				}
			}

			DWORD exceptionCode = e->ExceptionRecord->ExceptionCode ;
			fprintf( fp, "�������쳣: \n  ����: %x\n  ��ַ: 0x%x\n", exceptionCode ,e->ExceptionRecord->ExceptionAddress) ;
			DumpExceptionDescr( fp, exceptionCode ) ;

			DumpExceptCallsStack( fp, e->ContextRecord->Ebp,e->ContextRecord->Eip ) ;  
			fprintf(fp, "\n\n\n" );

			fflush(fp );
			fclose(fp );
		}
	}
	CSaveToDBThread thread;
	if( thread.Start() )
	{
		WaitForSingleObject( thread.getHandle(), INFINITE ); 
	}

	return EXCEPTION_EXECUTE_HANDLER;
}
//------------------------------------------------------------------------