/**
*	created:		2013-4-11   20:58
*	filename: 		FKPlatform
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKStructures.h"
#include "FKEvents.h"
//------------------------------------------------------------------------
namespace FK2DEngine2
{
	namespace Platform
	{
		// ����ָ������
		FK_EXPORT void Sleep( unsigned int iMS );

		// ����ָ�����״̬ ״̬����μ� FKStructures.h
		FK_EXPORT void SetCursor( unsigned char iCursor );

		// ��ȡ��ǰ���λ��
		FK_EXPORT void GetCursorPos( FK2DEngine2::Point & p );

		// ��ȡ�����С
		FK_EXPORT void GetDesktopSize( int & w, int & h );

		// ���� / ճ��ʱʹ��
		FK_EXPORT UnicodeString GetClipboardText();
		FK_EXPORT bool SetClipboardText( const UnicodeString & str );

		// ��ȡ��ǰ��ȷʱ��
		FK_EXPORT float GetTimeInSeconds();

		// ϵͳ���
		FK_EXPORT bool FileOpen( const String & Name, const String & StartPath, const String & Extension,
			FK2DEngine2::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback );
		FK_EXPORT bool FileSave( const String & Name, const String & StartPath, const String & Extension,
			FK2DEngine2::Event::Handler* pHandler, Event::Handler::FunctionWithInformation fnCallback );
		FK_EXPORT bool FolderOpen( const String & Name, const String & StartPath, FK2DEngine2::Event::Handler* pHandler,
			Event::Handler::FunctionWithInformation fnCallback );

		// �������
		FK_EXPORT void* CreatePlatformWindow( int x, int y, int w, int h, const FK2DEngine2::String & strWindowTitle );
		FK_EXPORT void DestroyPlatformWindow( void* pPtr );
		FK_EXPORT void SetBoundsPlatformWindow( void* pPtr, int x, int y, int w, int h );
		FK_EXPORT void MessagePump( void* pWindow, FK2DEngine2::Controls::Canvas* ptarget );
		FK_EXPORT bool HasFocusPlatformWindow( void* pPtr );
		FK_EXPORT void SetWindowMaximized( void* pPtr, bool bMaximized, FK2DEngine2::Point & pNewPos, FK2DEngine2::Point & pNewSize );
		FK_EXPORT void SetWindowMinimized( void* pPtr, bool bMinimized );
	}
}
//------------------------------------------------------------------------