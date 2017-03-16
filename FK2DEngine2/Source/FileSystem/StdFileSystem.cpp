/**
*	created:		2012-6-16   0:32
*	filename: 		StdFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../Include/FileSystem/FileSysCommon.h"
#include "../../Include/FileSystem/StdFileObject.h"
#include "../../Include/FileSystem/PackFileObject.h"
#include "../../Include/FileSystem/StdFileSystem.h"
#include <io.h>
//------------------------------------------------------------------------
// ���캯��
CStdFileSystem::CStdFileSystem()
{
	m_dwFilePos = m_dwBlockCount = m_dwLastBlockOffset = 0;
	m_pPackFile = NULL;
	if (!m_MapTable.empty())
		m_MapTable.clear();
	m_strFullPathName = "";
	m_strRootDirectory = "";
}
//------------------------------------------------------------------------
// ��ʼ��
BOOL CStdFileSystem::Init(LPCSTR szFileSystemName)
{
	// Ĭ���õ�ǰĿ¼
	char szPath[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, szPath);
	m_strRootDirectory = szPath;
	if (*(m_strRootDirectory.end() - 1) != '\\')
		m_strRootDirectory += '\\';
	
	// Ĭ�ϵ�ǰĿ¼
	if (szFileSystemName == NULL || 
		*szFileSystemName == '.' || 
		*szFileSystemName == 0) 
		return TRUE;
	else // ��Ĭ��
	{
		// �Ǿ���·�����������Ϊ\*�����ڶԸ�Ŀ¼������·���ķ���
		if (szFileSystemName[1] == ':' || 
		(szFileSystemName[0] == '\\' && szFileSystemName[1] == '\\') ||
		(szFileSystemName[0] == '/' && szFileSystemName[1] == '/')) 
		{
			m_strRootDirectory = szFileSystemName;
			if (lstrlenA(szFileSystemName) <= 3 && szFileSystemName[1] == ':')
				return TRUE;
		}
		else
			m_strRootDirectory += szFileSystemName;
		if (*(m_strRootDirectory.end() - 1) == '*')
			m_strRootDirectory.erase(m_strRootDirectory.end() - 1);
		if (*(m_strRootDirectory.end() - 1) == '\\')
			m_strRootDirectory.erase(m_strRootDirectory.end() - 1);
		m_strRootDirectory += "\\*";
		if (!DirectoryExist(m_strRootDirectory.c_str()))
		{
			if (!MakeDirectory(szFileSystemName))
			{
				TraceLn("����CStdFileSystem::Init()�����쳣��·��[%s]�����ڣ������ļ�ʧ�ܡ�\r\n",
					szFileSystemName);
				return FALSE;
			}
		}
		m_strRootDirectory.erase(m_strRootDirectory.end() - 1);
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// ��������
CStdFileSystem::~CStdFileSystem()
{

}
//------------------------------------------------------------------------
// �����ļ�(�����·��)
IFileObject* CStdFileSystem::CreateFile(LPCSTR szFileName, 
										char* pData /*= NULL*/, 
										int nSize /*= 0*/, 
										UINT nStyle/* = CFS_NEW*/)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	CStdFileObject* pStdFileObject = new CStdFileObject;
	ASSERT(pStdFileObject != NULL);

	pStdFileObject->SetFilePath(GetFullPathName(szFileName));

	if (nStyle == CFS_NEW) // �½�
	{
		if (pStdFileObject->IsExist()) // �ļ�������ʧ��
		{
			delete pStdFileObject;
			pStdFileObject = NULL;
			TraceLn("���󣺴����ļ������쳣�� �ļ� %s �Ѿ����ڣ�", szFileName);
			return NULL;
		}
		// �������ļ�(Ĭ�ϲ���,�û��Լ����ô�ģʽ����)
		pStdFileObject->Open("w");
		pStdFileObject->Close();
	}
	else if (nStyle == CFS_UPDATE) // ����
	{
		if (!pStdFileObject->IsExist()) // �ļ���������ʧ��
		{
			delete pStdFileObject;
			pStdFileObject = NULL;
			TraceLn("���󣺴����ļ������쳣�� �ļ� %s �����ڣ�", szFileName);
			return NULL;
		}
		// �������ļ�(Ĭ�ϲ���,�û��Լ����ô�ģʽ����)
		pStdFileObject->Open("w");
		pStdFileObject->Close();
	}
	return static_cast<IFileObject*>(pStdFileObject);
}
//------------------------------------------------------------------------
// ���ĳ���ļ�����(�����򿪶�Ӧ�ļ�!)
IFileObject* CStdFileSystem::GetFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	CStdFileObject* pStdFileObject = new CStdFileObject;
	ASSERT(pStdFileObject != NULL);

	pStdFileObject->SetFilePath(GetFullPathName(szFileName));
	return static_cast<IFileObject*>(pStdFileObject);
}
//------------------------------------------------------------------------
// ɾ��ָ�����ļ�
BOOL CStdFileSystem::DeleteFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	if (::DeleteFileA(GetFullPathName(szFileName)) == 0) // fails
	{
		DWORD dwErr = ::GetLastError();
		if (dwErr == 2 || dwErr == 3) // err:ϵͳ�Ҳ���ָ�����ļ�(·��)
		{
			TraceLn("����ɾ���ļ�ʱ�����쳣���ļ� %s �����ڣ�", szFileName);
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// �ļ�(Ŀ¼)������
// �ļ����Ըı�ԭ��·��,Ŀ¼��֧��
// ����Ĳ���������ΪNULL����ַ���
BOOL CStdFileSystem::Rename(LPCSTR szOldFileName,LPCSTR szNewFileName)	
{
	ASSERT(szOldFileName != NULL && szNewFileName != NULL &&
			*szOldFileName != 0 && *szNewFileName != 0);

	string old_name = GetFullPathName(szOldFileName);
	CStdFileObject* pStdFileObject = new CStdFileObject;
	pStdFileObject->SetFilePath(old_name.c_str());
	if (!pStdFileObject->IsExist()) // ԭʼ�ļ������Ͳ�����!
	{
		pStdFileObject->Release();
		pStdFileObject = NULL;
		TraceLn("�����ļ������������쳣���ļ� %s �����ڣ�", szOldFileName);
		return FALSE;
	}
	pStdFileObject->Release();
	pStdFileObject = NULL;

	string new_name = GetFullPathName(szNewFileName);
	if (/*rename*/::MoveFileA(old_name.c_str(), new_name.c_str()) == 0)
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------
// ����Ŀ¼
BOOL CStdFileSystem::CreateDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);
	return MakeDirectory(GetFullPathName(szDirectoryName));
}
//------------------------------------------------------------------------
// ɾ��Ŀ¼
BOOL CStdFileSystem::DeleteDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);
	return DeleteDirectoryEx(GetFullPathName(szDirectoryName));
}
//------------------------------------------------------------------------
// 1����Ŀ¼̬�ļ�ϵͳ���Ƴɰ�̬�ļ�ϵͳ���򷴹���
// 2������ǰ�ļ���ϵͳ���Ƶ���һ���ļ���ϵͳ��
// ���ڰ��ļ�ϵͳ�У�������ɾʱ���ֺܶ�ն������Ƶ����ļ���ϵͳ���Խ���������
BOOL CStdFileSystem::CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags/* = 0*/, PACKPROC lpfnPackProc/* = NULL*/)
{
	ASSERT(szNewFileSystemName != NULL && *szNewFileSystemName != 0);
	return DirToPack(szNewFileSystemName, lpfnPackProc);
}
//------------------------------------------------------------------------
// �ͷ�
BOOL CStdFileSystem::Release()
{
#ifdef _DEBUG
	char szbuf[256];
	wsprintfA(szbuf, "�ͷ��ļ�ϵͳ %s ...\r\n", m_strRootDirectory);
#endif
	if (m_pPackFile != NULL)
	{
		m_pPackFile->Release();
		m_pPackFile = NULL;
	}
#ifdef _DEBUG
	OutputDebugStringA(szbuf);
#endif
	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
// �����ļ�ϵͳ�ĳ�ʼ��Ŀ¼
BOOL CStdFileSystem::SetRootDir(LPCSTR szPath)
{
	if (szPath[1] != ':') // �Ƿ����·��
		return FALSE;

	if (!DirectoryExist(szPath)) // �Ƿ����
		return FALSE;

	m_strRootDirectory = szPath;

	// ���Ŀ¼�����һ����ĸ����'\',����������һ��'\'
	if (*(m_strRootDirectory.end() - 1) != '\\')
		m_strRootDirectory += '\\';

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileSystem::BrowseDir(LPCSTR szDir, PACKPROC lpfnPackProc/* = NULL*/)
{
	CStdFileObject* pStdFileObject = NULL;
	HANDLE hFile;
	WIN32_FIND_DATAA wfd;
	static int nBlockDepth = 0;

	//���Ȳ���szDirĿ¼�з���Ҫ����ļ�
	string strDir = szDir;

	if (*(strDir.end() - 1) != '\\') // ��������"\"
		strDir += '\\';

	string strFiles = strDir + "*.*";
	string strNextDir;

	// ���ȫ���ļ�
	if ((hFile = ::FindFirstFileA(strFiles.c_str(), &wfd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // ���ļ�
			{
				// �ҵ����ļ�������ļ�����
				pStdFileObject = new CStdFileObject();
				string strFileName = strDir;
				strFileName += wfd.cFileName;
				pStdFileObject->SetFilePath(strFileName.c_str());

				// newһ����ṹ�����
				BLOCKINFO* pBlockInfo = new BLOCKINFO();
				ASSERT(pBlockInfo != NULL);

				char strTmpFileName[MAX_RELATIVE_PATH_LEN];
				memset( strTmpFileName, 0, sizeof(strTmpFileName) );
				memcpy_s( strTmpFileName, MAX_RELATIVE_PATH_LEN, &strFileName[m_strRootDirectory.length()], strFileName.length() - m_strRootDirectory.length() );
				MAPTABLE mt(strTmpFileName, m_dwFilePos);
				pBlockInfo->bIsValid		= TRUE;
				pBlockInfo->u.bCompress		= FALSE;
				pBlockInfo->SetBlockName(wfd.cFileName);
				pBlockInfo->dwBlockSize		= wfd.nFileSizeLow;
				pBlockInfo->dwCompSize		= wfd.nFileSizeLow;
				pBlockInfo->dwOffset		= m_dwFilePos + sizeof(BLOCKINFO);
				pBlockInfo->dwPrevOffset	= m_dwLastBlockOffset;
				pBlockInfo->dwNextOffset	= m_dwFilePos + sizeof(BLOCKINFO) + wfd.nFileSizeLow;
				pBlockInfo->dwType			= CStdFileSystem::__FILE;
				pBlockInfo->dwVersion		= 1;
				m_dwLastBlockOffset = m_dwFilePos;
				m_dwFilePos += sizeof(BLOCKINFO) + wfd.nFileSizeLow; // �ļ�ָ��ƫ��

				// ����д����ļ�
				m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
				m_MapTable.push_back(mt);
				m_dwBlockCount ++;

				// ���ļ�����д����ļ�
				char* pFileBuf = new char[wfd.nFileSizeLow];
				pStdFileObject->Open();
				pStdFileObject->Read(pFileBuf, wfd.nFileSizeLow);
				m_pPackFile->Write(pFileBuf, wfd.nFileSizeLow);

				// �ص�
				if (lpfnPackProc)
					lpfnPackProc(1);

				// �ͷŻ���
				SAFE_DELETE(pFileBuf);
				SAFE_DELETE(pBlockInfo);
				SAFE_DELETE(pStdFileObject);
			}
		}
		while (::FindNextFileA(hFile, &wfd));

		::FindClose(hFile);
	}


	// ���ȫ��Ŀ¼
	if ((hFile = ::FindFirstFileA(strFiles.c_str(), &wfd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ��Ŀ¼
			{
				if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0)
				{
					nBlockDepth ++;

					// ������һ���Ŀ¼��
					strNextDir = strDir + wfd.cFileName;
					
					// newһ����ṹ�����
					BLOCKINFO* pBlockInfo = new BLOCKINFO();
					ASSERT(pBlockInfo != NULL);

					char strTmpFileName[MAX_RELATIVE_PATH_LEN];
					memset( strTmpFileName, 0, sizeof(strTmpFileName) );
					memcpy_s( strTmpFileName, MAX_RELATIVE_PATH_LEN, &strNextDir[m_strRootDirectory.length()], strNextDir.length() - m_strRootDirectory.length() );
					MAPTABLE mt(strTmpFileName, m_dwFilePos);

					pBlockInfo->bIsValid		= TRUE;
					pBlockInfo->u.nBlockDepth	= nBlockDepth;
					pBlockInfo->SetBlockName(wfd.cFileName);
					pBlockInfo->dwBlockSize		= 0;
					pBlockInfo->dwCompSize		= 0;
					pBlockInfo->dwOffset		= 0;
					pBlockInfo->dwPrevOffset	= m_dwLastBlockOffset;
					pBlockInfo->dwNextOffset	= m_dwFilePos + sizeof(BLOCKINFO);
					pBlockInfo->dwType			= CStdFileSystem::__DIR;
					pBlockInfo->dwVersion		= 1;
					m_dwLastBlockOffset = m_dwFilePos;
					m_dwFilePos += sizeof(BLOCKINFO); // �ļ�ָ��ƫ��

					// ����д����ļ�
					m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
					m_MapTable.push_back(mt);
					m_dwBlockCount ++;

					// �ͷſ黺��
					SAFE_DELETE(pBlockInfo);

					// ���������һ��Ŀ¼(�ݹ�)
					BrowseDir(strNextDir.c_str(), lpfnPackProc);
					nBlockDepth --;
				}
			}

		}
		while (::FindNextFileA(hFile, &wfd));

		::FindClose(hFile);
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CStdFileSystem::DirToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc/* = NULL*/)
{
	m_pPackFile = new CStdFileObject();
	m_pPackFile->SetFilePath(szPackFileName);
	m_pPackFile->Open("wb");
	
	// д����ļ�ͷ
	PACKHEADER	ph;
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	// �ļ�ָ��ƫ��
	m_dwFilePos = 0;
	m_dwFilePos += sizeof(PACKHEADER);
	ph.dwBlockOffset = m_dwFilePos;

	// д��Ŀ¼��ʱȥ��·����Ϣ
	string strDirName = m_strRootDirectory;
	if (*(strDirName.end() - 1) == '\\') // ɾ�����һ��"\"
		strDirName.erase(strDirName.end() - 1); 
	int npos = strDirName.find_last_of("\\");
	strDirName.erase(strDirName.begin(), strDirName.begin() + npos + 1);

	// ��䶥��Ŀ¼�Ŀ���Ϣ
	BLOCKINFO* pBlockInfo = new BLOCKINFO();
	ASSERT(pBlockInfo != NULL);

	MAPTABLE mt(strDirName.c_str(), m_dwFilePos);

	m_dwLastBlockOffset = 0;

	pBlockInfo->bIsValid		= TRUE;
	pBlockInfo->u.nBlockDepth	= 0;
	pBlockInfo->SetBlockName(strDirName.c_str());
	pBlockInfo->dwBlockSize		= 0;
	pBlockInfo->dwCompSize		= 0;
	pBlockInfo->dwOffset		= 0;
	pBlockInfo->dwPrevOffset	= m_dwLastBlockOffset; // ǰ��û�п���Ϣͷ��!
	pBlockInfo->dwNextOffset	= m_dwFilePos + sizeof(BLOCKINFO);
	pBlockInfo->dwType			= CStdFileSystem::__DIR;
	pBlockInfo->dwVersion		= 1;
	m_dwLastBlockOffset = m_dwFilePos;
	m_dwFilePos += sizeof(BLOCKINFO); // �ļ�ָ��ƫ��

	m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
	m_MapTable.push_back(mt);
	m_dwBlockCount ++;

	SAFE_DELETE(pBlockInfo); // �ͷſ��ڴ�

	// �������Ŀ¼�µ����е��ļ�����Ŀ¼,��������д����ļ�
	BrowseDir(m_strRootDirectory.c_str(), lpfnPackProc);

	// д�ļ�ӳ����ļ�β
	m_pPackFile->Write(&m_MapTable[0], m_MapTable.size()*sizeof(MAPTABLE));

	// �޸����һ������Ϣͷ��dwNextOffsetֵ
	BLOCKINFO bi;
	m_pPackFile->Seek(m_dwLastBlockOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
	DWORD dwLastBlock_NextOffset = 0;
	m_pPackFile->Write(&dwLastBlock_NextOffset, sizeof(DWORD));

	// �޸İ��ļ�ͷ��ĳЩ����
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset = m_dwFilePos;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	// ���ļ��ر�
	m_pPackFile->Close();
	return TRUE;
}
//------------------------------------------------------------------------
// �ж�Ŀ¼�Ƿ����
BOOL CStdFileSystem::DirectoryExist(LPCSTR szDirectoryName)
{
	string strDir = szDirectoryName;
	if (*(strDir.end() - 1) == '\\') // ��ɾ������"\"
		*(strDir.end() - 1) = '\0';

	WIN32_FIND_DATAA wfd; // ����

	BOOL bExist = FALSE;
	HANDLE hFind=::FindFirstFileA(strDir.c_str(), &wfd); 
	if (hFind == INVALID_HANDLE_VALUE) // û���ҵ�ƥ�䣬Ŀ¼�϶�������
	{
		bExist = FALSE;
	}
	else
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ����ҵ��Ľ���Ƿ�Ŀ¼
			bExist = TRUE; // ��Ŀ¼,Ŀ¼����
		else // ����Ŀ¼,Ŀ¼������
			bExist = FALSE;
	}
	FindClose(hFind);
	return bExist;
}
//------------------------------------------------------------------------
// ����Ŀ¼(�����༶,szDirectoryName�����·����;�ݹ�)
// �Զ����Ŀ¼�Ƿ����
BOOL CStdFileSystem::MakeDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	if (DirectoryExist(szDirectoryName)) // Ŀ¼�Ѵ���
		return TRUE;

	string strDir = szDirectoryName;
	string strParent;

	if (*(strDir.end() - 1) == '\\') // ��ɾ������"\"
		strDir.erase(strDir.end() - 1, strDir.end());

	// �ֿ���Ŀ¼�ͱ���Ŀ¼����
	for (int i = strDir.length()-1; i > 0; i --)
	{
		if (strDir.at(i) == '\\' || strDir.at(i) == '/')
		{
			strParent = strDir.substr(0, i);
			break;
		}
	}

	if (strParent.empty()) // Ŀ¼���ƴ���
		return FALSE; 

	BOOL Ret = TRUE;
	if (strParent.length() > 3) // �������С��3����ʾΪ���̸�Ŀ¼
		Ret = DirectoryExist(strParent.c_str());// ��鸸Ŀ¼�Ƿ����

	if (!Ret) // ��Ŀ¼������,�ݹ���ô�����Ŀ¼
		Ret = MakeDirectory(strParent.c_str()); 

	if (Ret) // ��Ŀ¼����,ֱ�Ӵ���Ŀ¼
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength=sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		Ret = ::CreateDirectoryA(strDir.c_str(), &sa);
	}
	return Ret;
} 
//------------------------------------------------------------------------
// ɾ��Ŀ¼(�����༶��Ŀ¼����ļ�;�ݹ�)
// �Զ����Ҫɾ����Ŀ¼�Ƿ����,�����ڷ���TRUE,����ɾ������
BOOL CStdFileSystem::DeleteDirectoryEx(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	string strDir = szDirectoryName;

	// �������ȱ������3��������Ϊ���̸�Ŀ¼��հ�
	if (strDir.empty() || strDir.length() < 4)
		return FALSE;

	if (*(strDir.end() - 1) != '\\') // ��������"\"
		strDir += '\\';

	int len = strDir.length();

	string strFiles = strDir + "*.*";

	WIN32_FIND_DATAA wfd;
	HANDLE hFind = FindFirstFileA(strFiles.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE) // Ŀ¼������,���Ƿ���TRUE
	{
		TraceLn("���� CStdFileSystem::DeleteDirectoryEx() �����쳣��Ŀ¼ %s �����ڣ�", szDirectoryName);
		return TRUE; 
	}

	BOOL Ret = TRUE;
	string strTmp;
	if (hFind != INVALID_HANDLE_VALUE) // ���ڴ�Ŀ¼
	{
		BOOL bFind = TRUE;
		while (bFind)
		{
			if (wfd.cFileName[0] != '.') // . ..
			{
				strTmp = strDir + wfd.cFileName;
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ɾ��������Ŀ¼
				{ 
					Ret = Ret && DeleteDirectoryEx(strTmp.c_str());
				}
				else // ɾ�������ļ�
				{ 
					SetFileAttributesA(strTmp.c_str(), FILE_ATTRIBUTE_NORMAL);
					Ret = Ret && ::DeleteFileA(strTmp.c_str());
				}
			}
			bFind = FindNextFileA(hFind, &wfd);
		}
		FindClose(hFind);

		if (Ret)
		{
			BOOL b = ::RemoveDirectoryA(strDir.c_str());
			if (b)
			{
				return TRUE;
			}
			TraceLn("�����Ƴ��ļ���ʧ�ܡ� GetLastError() = %d", ::GetLastError());
			return FALSE;
		}
	}

	return FALSE;
} 
//------------------------------------------------------------------------
// ������·���ľ���·��(��������Ǿ���·��,���Թ�)
inline const char* CStdFileSystem::GetFullPathName(LPCSTR szRelativePathName)
{
	if (szRelativePathName[0]=='/' || 
		szRelativePathName[0]=='\\' || 
		szRelativePathName[1] == ':') // ������Ǿ���·��
		return szRelativePathName;

	m_strFullPathName = m_strRootDirectory;
	m_strFullPathName += szRelativePathName;
	return m_strFullPathName.c_str();
}
//------------------------------------------------------------------------