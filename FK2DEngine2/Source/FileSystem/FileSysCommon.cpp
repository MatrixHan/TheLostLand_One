/**
*	created:		2012-6-16   2:54
*	filename: 		FileSysCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../Include/FileSystem/FileSysCommon.h"
#include "../../Include/FileSystem/StdFileSystem.h"
#include "../../Include/FileSystem/StdFileObject.h"
#include "../../Include/FileSystem/PackFileSystem.h"
//------------------------------------------------------------------------
// ������� : CreateFileSystem
// ������
// szFileSystemName : 	����Ŀ¼̬ϵͳ,����ĳ���ļ����ڵ�Ŀ¼����e:\dxGame\data\bmp
//						���ڰ�̬�ļ�ϵͳ,���Ǹ����ļ�
// dwStyle			: 	ָ���Ǵ���Ŀ¼̬�ļ�ϵͳ���ǰ�̬�ļ�ϵͳ��������ָ������ѡ��
BOOL CIFileSystemHelper::CreateFileSystem(DWORD dwExpectVersion,
													   LPCSTR szFileSystemName, DWORD dwStyle, ITrace* pTrace,
													   IFileSystem** ppFileSystem,	void* Reserved)
{
	if(dwExpectVersion != VERSION_QUERY_FILE_SYSTEM)
	{
		TraceLn("CreateFileSystem() : Version error!");
		return FALSE;
	}

	if (dwStyle != FSS_DIRECTORY && dwStyle != FSS_PACKAGE)
		dwStyle = FSS_DIRECTORY;

	// ���ݴ���Ĳ�������ʶ���ļ�ϵͳ����
	if (szFileSystemName == NULL || 
		*szFileSystemName == '.' || 
		*szFileSystemName == 0)  // �����õ�ǰĿ¼��Ŀ¼̬
		dwStyle = FSS_DIRECTORY;
	else
	{
		char buf[1024];
		int nLen = lstrlenA(szFileSystemName);
		lstrcpynA(buf, szFileSystemName, 1024);
		if (buf[nLen-1] == '\\' || buf[nLen-1] == '/')
			buf[nLen-1] = '\0';

		WIN32_FIND_DATAA wfd;
		if (INVALID_HANDLE_VALUE != ::FindFirstFileA(buf, &wfd)) // ����
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Ŀ¼��˵����Ŀ¼̬
				dwStyle = FSS_DIRECTORY;
			else  // ˵���ǰ�̬
				dwStyle = FSS_PACKAGE;
		}
		else // �����ڣ�˵�����½�����������dwStyle����
		{
			if (-1 != _access(buf, 0)) // ���ڣ�˵���Ǹ�Ŀ¼
			{
				dwStyle = FSS_DIRECTORY;
			}

		}

	}

	if (dwStyle == FSS_DIRECTORY)
	{
		CStdFileSystem*	pStdFileSystem = new CStdFileSystem();
		ASSERT(pStdFileSystem);
		if (!pStdFileSystem->Init(szFileSystemName))
		{
			SAFE_DELETE(pStdFileSystem);
			return FALSE;
		}
		*ppFileSystem = static_cast<IFileSystem*>(pStdFileSystem);
	}
	else if (dwStyle == FSS_PACKAGE)
	{
		CPackFileSystem* pPackFileSystem = new CPackFileSystem();
		ASSERT(pPackFileSystem);
		if (!pPackFileSystem->Init(szFileSystemName))
		{
			SAFE_DELETE(pPackFileSystem);
			return FALSE;
		}
		*ppFileSystem = static_cast<IFileSystem*>(pPackFileSystem);
	}

	TraceLn("CreateFileSystem('%s')......OK", szFileSystemName);
	return *ppFileSystem != NULL;
}
//------------------------------------------------------------------------
// ����ֻ�ṩ������׼�ļ�����
BOOL CIFileSystemHelper::CreateFileObject(DWORD dwExpectVersion,
													   LPCSTR szFileName, ITrace* pTrace,
													   IFileObject** ppFileObject)
{
	if(dwExpectVersion != VERSION_QUERY_FILE_SYSTEM)
	{
		TraceLn("CreateFileObject() : Version error!");
		return FALSE;
	}

	CStdFileObject* pStdFileObject = new CStdFileObject;
	ASSERT(pStdFileObject != NULL);

	pStdFileObject->SetFilePath(szFileName);
	*ppFileObject = static_cast<IFileObject*>(pStdFileObject);
	TraceLn("CreateFileObject()......OK");
	return pStdFileObject != NULL;
}
//------------------------------------------------------------------------