/**
*	created:		2013-4-16   1:02
*	filename: 		xFile
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// ��ȡһ���ļ���С
inline int	_GetFileSize( FILE * fp )
{
	if( fp == NULL )return 0;
	int oldfp = ftell( fp );
	fseek( fp , 0, SEEK_END );
	int ret = ftell( fp );
	fseek( fp, oldfp, SEEK_SET );
	return ret;
}
//------------------------------------------------------------------------
// ��һ���ļ�
inline BYTE * LoadFile( const char * pszFileName )
{
	FILE	*	fp = fopen( pszFileName, "rb" );
	if( fp == NULL )return NULL;
	BYTE	*	pBytes = NULL;
	int size = _GetFileSize( fp );
	if( size == 0 )return NULL;
	pBytes = new BYTE[size+16];
	fread( pBytes, size, 1, fp );
	fclose( fp );
	pBytes[size] = 0;
	return pBytes;
}
//------------------------------------------------------------------------
// ��ȡһ���ļ�
inline BYTE * LoadFile( const char * pszFileName, int & size )
{
	FILE	*	fp = fopen( pszFileName, "rb" );
	if( fp == NULL )return NULL;
	BYTE	*	pBytes = NULL;
	size = _GetFileSize( fp );
	if( size == 0 )return NULL;
	pBytes = new BYTE[size+16];
	fread( pBytes, size, 1, fp );
	fclose( fp );
	pBytes[size] = 0;
	return pBytes;
}
//------------------------------------------------------------------------
// ���һ��·���Ƿ����ļ���
inline BOOL		PathIsFolder( const char * pszPath )
{
	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	HANDLE	hFind = FindFirstFile( pszPath, &wfd );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		return ((wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)== FILE_ATTRIBUTE_DIRECTORY);
	}
	return FALSE;
}
//------------------------------------------------------------------------
// ���һ���ļ��Ƿ����
inline BOOL		FileExist( const char * pszPath )
{
	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	HANDLE	hFind = FindFirstFile( pszPath, &wfd );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
		return TRUE;
	}
	return FALSE;	
}
//------------------------------------------------------------------------