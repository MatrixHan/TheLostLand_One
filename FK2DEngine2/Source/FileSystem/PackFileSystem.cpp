/**
*	created:		2012-6-16   1:46
*	filename: 		PackFileSystem
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
#include "../../Include/FileSystem/PackFileSystem.h"
//------------------------------------------------------------------------
// ���캯��
CPackFileSystem::CPackFileSystem()
{
	m_dwFilePos = m_dwBlockCount = 0;
	m_pPackFile = NULL;
	if (!m_Hash.empty())
		m_Hash.clear();
	m_strFullPathName = "";
	m_strRootDirectory = "";
	m_itCurFile = m_Hash.begin();
}
//------------------------------------------------------------------------
// ��������
CPackFileSystem::~CPackFileSystem()
{

}
//------------------------------------------------------------------------
// ��ʼ��
BOOL CPackFileSystem::Init(LPCSTR szFileSystemName)
{
	ASSERT(szFileSystemName != NULL);

	// �����µİ��ļ����󣬲��������ж�д����
	m_pPackFile = new CStdFileObject();
	m_pPackFile->SetFilePath(szFileSystemName);
	if (!m_pPackFile->IsExist()) // �����ڣ����½�
	{
#ifndef _DEBUG
		char buf[256];
		wsprintf(buf, "�ļ�ϵͳ������(%s)!", szFileSystemName);
		::MessageBox(NULL, buf, "ERROR", MB_OK);
		TraceLn("���� CPackFileSystem::Init() �����쳣���ļ� %s �����ڣ�", szFileSystemName);		
		m_pPackFile->Release();
		return FALSE;
#else
		string	str = szFileSystemName;
		int nPos = str.find_last_of("\\");
		if (nPos != -1)
		{
			str.erase(str.begin() + nPos, str.end());
			if (!DirectoryExist(str.c_str()) && !MakeDirectory(str.c_str()))
				return FALSE;
		}
		m_pPackFile->Open("wb");

		// д����ļ�ͷ
		PACKHEADER	ph;
		ph.dwBlockCount = 1;
		ph.dwBlockOffset = sizeof(PACKHEADER);	
		ph.dwMapOffset = sizeof(PACKHEADER) + sizeof(BLOCKINFO);
		m_pPackFile->Write(&ph, sizeof(PACKHEADER));

		// д��Ŀ¼��ʱȥ��·����Ϣ
		char strDirName[MAX_PATH];
		m_pPackFile->GetFileTitle(strDirName);

		// ��䶥��Ŀ¼�Ŀ���Ϣ
		BLOCKINFO* pBlockInfo = new BLOCKINFO();
		ASSERT(pBlockInfo != NULL);
		pBlockInfo->bIsValid		= TRUE;
		pBlockInfo->u.nBlockDepth	= 0;
		pBlockInfo->SetBlockName(strDirName);
		pBlockInfo->dwBlockSize		= 0;
		pBlockInfo->dwCompSize		= 0;
		pBlockInfo->dwOffset		= 0;
		pBlockInfo->dwPrevOffset	= 0; // ǰ��û�п���Ϣͷ��!
		pBlockInfo->dwNextOffset	= 0;
		pBlockInfo->dwType			= 0;
		pBlockInfo->dwVersion		= 1;
		m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
		SAFE_DELETE(pBlockInfo); // �ͷſ��ڴ�

		// д�ļ�ӳ����ļ�β
		MAPTABLE mt(strDirName, sizeof(PACKHEADER));
		m_pPackFile->Write(&mt, sizeof(MAPTABLE));

		// ���ļ��ر�
		m_pPackFile->Close();
#endif		
	}
	if (GetFileAttributesA(szFileSystemName) & FILE_ATTRIBUTE_READONLY )
	{
		if (!SetFileAttributesA(szFileSystemName, FILE_ATTRIBUTE_NORMAL))
		{
			TraceLn("����CPackFileSystem::Init() �����쳣���ļ� %s ���������ļ����� SetFileAttributes failed ��", szFileSystemName);
			return FALSE;
		}
	}
	if (!m_pPackFile->Open("r+b"))
	{
		TraceLn("����CPackFileSystem::Init() �����쳣���ļ� %s �޷��򿪣�", szFileSystemName);
		return FALSE;
	}
	m_strRootDirectory = szFileSystemName;
	int pos = m_strRootDirectory.find_last_of('\\');
	m_strRootDirectory.erase(m_strRootDirectory.begin() + pos + 1, m_strRootDirectory.end());

	// ��ȡ�ļ�ӳ���
	return UpdateMapTable();
}
//------------------------------------------------------------------------
BOOL CPackFileSystem::UpdateMapTable()
{
	// ��ȡ�ļ�ӳ���
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ����ʶ�Ƿ���ȷ
	{
		m_pPackFile->Close();
		m_pPackFile->Release();
		m_pPackFile = NULL;
		return FALSE;
	}

	// ��ʱ��CreateFile����û���������������������ӳ������һЩǿУ��
	if (ph.dwReserved1 == 1)
	{
		if (m_pPackFile->GetLength() != 
			ph.dwMapOffset + sizeof(MAPTABLE)*ph.dwBlockCount)
		{
			TraceLn("����CPackFileSystem::UpdateMapTable() �����쳣���ļ� %s ӳ���ƫ��ֵ�Ƿ���", m_strRootDirectory.c_str());
			return FALSE;
		}
	}
	m_dwBlockCount = ph.dwBlockCount;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	// ����hash��
	if (!m_Hash.empty())
		m_Hash.clear();
	for (int i = 0; i < ph.dwBlockCount; i ++)
		AddToHash(pmt[i].name, pmt[i].offset);

	m_itCurFile = m_Hash.begin();
	// ������Ŀ¼��
	BLOCKINFO bi;
	m_pPackFile->Seek(ph.dwBlockOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
	{
		delete[] pmt;
		m_Hash.clear();
		return FALSE;
	}
	m_strRootDirectory += bi.cbName;
	m_strRootDirectory += "\\";

	// Seek to begin
	m_pPackFile->SeekToBegin();

	delete[] pmt;
	return TRUE;
}
//------------------------------------------------------------------------
// �����ļ�(�����·��)
IFileObject* CPackFileSystem::CreateFile(LPCSTR szFileName, 
										 char* pData /*= NULL*/, 
										 int nSize /*= 0*/, 
										 UINT nStyle/* = CFS_NEW*/)
{
	ASSERT(szFileName != NULL && *szFileName != 0);
	if (pData == NULL || nSize <= 0)
		return NULL;

	// �ļ��Ƿ����?���ڷ���NULL?
	DWORD Offset = GetOffsetFromHash(szFileName);
	if (Offset)
	{
		BLOCKINFO bi;
		m_pPackFile->Seek(Offset, SEEK_SET);
		m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
		if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // ���ǿ���Ϣͷ,���Ϊ��Ч,Ŀ¼
		{
			TraceLn("CPackFileSystem::CreateFile()���ļ���Ϣͷ��Ч");
			return NULL;
		}

		if (bi.dwCompSize >= nSize) // ԭ�ظ���
		{
			bi.dwBlockSize = nSize;
			bi.dwCompSize = nSize;
			m_pPackFile->Seek(Offset, SEEK_SET);
			m_pPackFile->Write(&bi, sizeof(BLOCKINFO));
			m_pPackFile->Write(pData, nSize);// �����ݸ���
			return GetFile(szFileName);
		}
		else // ɾ�����ļ������½���һ���ļ�
		{
			if (!DeleteFile(szFileName))
			{
				TraceLn("CPackFileSystem::CreateFile() ɾ��ԭ�ļ� %s ʧ�ܣ�", szFileName );
				return NULL;
			}
			return CreateFile(szFileName, pData, nSize, nStyle);
		}
	}

	// �������ļ�
	// ��ȡ�������丸Ŀ¼��Ӧ��·����
	char szBlockName[MAX_BLOCK_NAME_LEN];
	char szParentPathName[MAX_RELATIVE_PATH_LEN];
	string strFileName = GetFullPathName(szFileName);
	int pos = strFileName.find_last_of('\\');
	lstrcpyA(szBlockName, string( strFileName.substr( pos + 1, strFileName.length() - pos - 1 ) ).c_str() );
	string strParentDir = strFileName;
	strParentDir.erase(strParentDir.begin() + pos, strParentDir.end());
	if (strParentDir.length() == m_strRootDirectory.length() - 1) // û�и�Ŀ¼
	{
		int n = strParentDir.find_last_of('\\');
		lstrcpyA(szParentPathName, string( strFileName.substr( n + 1, strFileName.length() - n - 1 ) ).c_str() );
	}
	else
	{
		lstrcpyA(szParentPathName, string( strFileName.substr( m_strRootDirectory.length(), strFileName.length() - m_strRootDirectory.length() ) ).c_str() );
	}
	
	// ��ȡ�丸Ŀ¼�Ŀ���Ϣ
	BLOCKINFO bi;
	DWORD dwParentOffset = GetOffsetFromHash(szParentPathName);
	if (!dwParentOffset) // ��Ŀ¼������!�ȴ�����Ŀ¼��!!!
	{
		TraceLn("���� CPackFileSystem::CreateFile() �����쳣��������Ŀ¼ʧ�ܣ���Ŀ¼�����ڣ�");
		return NULL;
	}

	// ��ȡ��Ŀ¼�Ŀ���Ϣ�����޸�������һ�����ƫ��ֵ
	m_pPackFile->Seek(dwParentOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
		return NULL;

	// ��ȡ�ļ�ӳ���
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ���ǰ��ļ�ͷ
		return NULL;
	// дһ����־����CreateFile˳�����������������־
	ph.dwReserved1 = 1;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));
	m_pPackFile->Flush();

	// ���޸ĺ�ĸ�Ŀ¼����Ϣ��д
	DWORD dwNextOffset = bi.dwNextOffset;
	bi.dwNextOffset = ph.dwMapOffset;
	m_pPackFile->Seek(dwParentOffset, SEEK_SET);
	m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);
	pPackFileObject->SetFilePath(GetFullPathName(szFileName));

	// ����ӳ���
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, ph.dwBlockCount*sizeof(MAPTABLE));

	// newһ����ṹ�����
	BLOCKINFO* pBlockInfo = new BLOCKINFO();
	ASSERT(pBlockInfo != NULL);
	
	// ��¼ӳ����Ϣ
	MAPTABLE mt( string( strFileName.substr( m_strRootDirectory.length(), strFileName.length() - m_strRootDirectory.length() ) ).c_str(), ph.dwMapOffset);

	pBlockInfo->bIsValid		= TRUE;
	pBlockInfo->u.bCompress		= FALSE;
	pBlockInfo->SetBlockName(szBlockName);
	pBlockInfo->dwBlockSize		= nSize;
	pBlockInfo->dwCompSize		= nSize;
	pBlockInfo->dwOffset		= ph.dwMapOffset + sizeof(BLOCKINFO);
	pBlockInfo->dwPrevOffset	= dwParentOffset;
	pBlockInfo->dwNextOffset	= dwNextOffset;
	pBlockInfo->dwType			= CPackFileSystem::__FILE;
	pBlockInfo->dwVersion		= 1;

	// ��д��һ������Ϣ��dwPrevOffset
	if (dwNextOffset != 0) // ˵�����鲻�����һ����
	{
		m_pPackFile->Seek(dwNextOffset + offsetof(BLOCKINFO, dwPrevOffset), SEEK_SET);
		m_pPackFile->Write(&ph.dwMapOffset, sizeof(DWORD));
	}

	// ����д����ļ�
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
	AddToHash(mt.name, mt.offset);
	m_itCurFile = m_Hash.begin();

	// ���ļ�����д����ļ�
	m_pPackFile->Write(pData, nSize);

	// ��дӳ���
	BOOL bFound = FALSE;
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwParentOffset) // �ҵ���Ŀ¼�Ķ�Ӧ��!
		{
			bFound = TRUE;
			m_pPackFile->Seek(ph.dwMapOffset + sizeof(BLOCKINFO) + nSize, SEEK_SET); // ��λ��
			m_pPackFile->Write(pmt, (i + 1)*sizeof(MAPTABLE)); // д��Ŀ¼��ǰ��(������Ŀ¼)
			m_pPackFile->Write(&mt, sizeof(MAPTABLE));// д�մ������ļ���ӳ���
			m_pPackFile->Write(&pmt[i + 1], (ph.dwBlockCount - i -1)*sizeof(MAPTABLE));// д�´����ļ������ӳ���
			break;
		}
	}
	if (!bFound) // û�ҵ�!����ʧ��
	{
		pPackFileObject->Release();
		pPackFileObject = NULL;
		SAFE_DELETE(pBlockInfo);
		delete[] pmt;
		return NULL;
	}

	// �����ļ�����
	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), ph.dwMapOffset, nSize);

	// �޸İ��ļ�ͷ
	m_dwBlockCount ++;
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset += sizeof(BLOCKINFO) + nSize;
	ph.dwReserved1 = 0;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));
	m_pPackFile->Flush();

	// �ͷŻ���
	SAFE_DELETE(pBlockInfo);
	delete[] pmt;
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
// ���ĳ���ļ�����(�����򿪶�Ӧ�ļ�!)
IFileObject* CPackFileSystem::GetFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	DWORD dwOffset = GetOffsetFromHash(szFileName);
	if (!dwOffset) // ��ϣ���в�����!
		return NULL;

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // ���ǿ���Ϣͷ,���Ϊ��Ч,Ŀ¼
	{
		return NULL;
	}

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName(szFileName));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, bi.dwCompSize);
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
// ���ĳ���ļ�����(�����򿪶�Ӧ�ļ�!)
IFileObject* CPackFileSystem::GetIndexFile(LPCSTR szFileName, DWORD dwCompSize)
{
	ASSERT(szFileName != NULL && *szFileName != 0);

	DWORD dwOffset = GetOffsetFromHash(szFileName);
	if (!dwOffset) // ��ϣ���в�����!
		return NULL;

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName(szFileName));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, dwCompSize);
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
IFileObject* CPackFileSystem::GetFirstFile()
{
	if(m_Hash.size() == 0)
		return NULL;
	
	m_itCurFile = m_Hash.begin();
	
	DWORD dwOffset = (*m_itCurFile).second;
	if (!dwOffset) // ��ϣ���в�����!
		return NULL;

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // ���ǿ���Ϣͷ,���Ϊ��Ч,Ŀ¼
	{
		return NULL;
	}

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName((*m_itCurFile).first.c_str()));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, bi.dwCompSize);

	m_itCurFile++;
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
IFileObject* CPackFileSystem::GetNextFile()
{
	if(m_itCurFile == m_Hash.end())
		return NULL;

	DWORD dwOffset = (*m_itCurFile).second;
	if (!dwOffset) // ��ϣ���в�����!
		return NULL;

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid() || !bi.bIsValid || bi.dwType == CPackFileSystem::__DIR) // ���ǿ���Ϣͷ,���Ϊ��Ч,Ŀ¼
	{
		return NULL;
	}

	CPackFileObject* pPackFileObject = new CPackFileObject;
	ASSERT(pPackFileObject != NULL);

	pPackFileObject->SetFilePath(GetFullPathName((*m_itCurFile).first.c_str()));

	pPackFileObject->SetFileInfo(m_pPackFile->GetHandle(), dwOffset, bi.dwCompSize);

	m_itCurFile++;
	return static_cast<IFileObject*>(pPackFileObject);
}
//------------------------------------------------------------------------
// ɾ��ָ�����ļ�
BOOL CPackFileSystem::DeleteFile(LPCSTR szFileName)
{
	ASSERT(szFileName != NULL && *szFileName != 0);


	DWORD dwOffset = GetOffsetFromHash(szFileName);
	if (!dwOffset) // ��ϣ���в�����!Ȩ�ҵ���ɾ����
	{
		TraceLn("����CPackFileSystem::DeleteFile() �����쳣���ļ� %s �����ڣ�", szFileName);
		return TRUE;
	}

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
		return FALSE;

	// �޸�ǰһ���dwNextOffsetֵ
	if (bi.dwPrevOffset != 0)
	{
		m_pPackFile->Seek(bi.dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
		m_pPackFile->Write(&bi.dwNextOffset, sizeof(DWORD));
	}

	// �޸ĺ�һ���dwPrevOffsetֵ
	if (bi.dwNextOffset != 0)
	{
		m_pPackFile->Seek(bi.dwNextOffset + offsetof(BLOCKINFO, dwPrevOffset), SEEK_SET);
		m_pPackFile->Write(&bi.dwPrevOffset, sizeof(DWORD));
	}

	// ��������Ϊ��Ч
	BOOL bIsValid = FALSE;
	m_pPackFile->Seek(dwOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
	m_pPackFile->Write(&bIsValid, sizeof(BOOL));

	// ɾ��ӳ���Ķ�Ӧ��
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ���ǰ��ļ�ͷ
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);
	BOOL bFound = FALSE;
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwOffset) // �ҵ�!
		{
			bFound = TRUE;
			m_pPackFile->Seek(ph.dwMapOffset + i*sizeof(MAPTABLE), SEEK_SET);
			m_pPackFile->Write(&pmt[i + 1], (ph.dwBlockCount - i -1)*sizeof(MAPTABLE)); // �������һ��ӳ����û��ɾ��!!
			break;
		}
	}
	SAFE_DELETE(pmt);
	if (!bFound) // û�ҵ�!!
		return FALSE;

	// ɾ����ϣ���еĶ�Ӧ��
	EraseFromHash(szFileName);

	// �޸İ��ļ�ͷ
	m_dwBlockCount --;
	ph.dwBlockCount = m_dwBlockCount;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	TraceLn("DeleteFile     : %s", szFileName);
	return TRUE;
}
//------------------------------------------------------------------------
// �ļ�(Ŀ¼)������
// �ļ����Ըı�ԭ��·��,Ŀ¼��֧��
// ����Ĳ���������ΪNULL����ַ���
BOOL CPackFileSystem::Rename(LPCSTR szOldFileName,LPCSTR szNewFileName)	
{
	ASSERT(szOldFileName != NULL && szNewFileName != NULL &&
			*szOldFileName != 0 && *szNewFileName != 0);

	DWORD dwOffset = GetOffsetFromHash(szOldFileName);
	if (!dwOffset) // ��ϣ���в�����!
	{
		TraceLn("����CPackFileSystem::Rename()�����쳣���ļ� %s �����ڣ�", szOldFileName);
		return FALSE;
	}

	// �޸Ŀ��ж�Ӧ����Ϣ
	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
		return FALSE;
	string str = szNewFileName;
	if (*(str.end() - 1) == '\\') // ��ɾ������"\"
		str.erase(str.end() - 1, str.end());
	int pos = str.find_last_of('\\');
	lstrcpyA(bi.cbName, string( str.substr( pos + 1, str.length() - pos - 1 ) ).c_str() ); // ����
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

	// �޸Ĺ�ϣ��
	EraseFromHash(szOldFileName);
	AddToHash(str.c_str(), dwOffset);
	m_itCurFile = m_Hash.begin();

	// �޸�ӳ���
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ���ǰ��ļ�ͷ
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwOffset)
		{
			lstrcpyA(pmt[i].name, str.c_str());
			break;
		}
	}
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Write(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	SAFE_DELETE(pmt);

	return TRUE;
}
//------------------------------------------------------------------------
// ����Ŀ¼
BOOL CPackFileSystem::CreateDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	// Ŀ¼�Ƿ����?���ڷ���TRUE?
	DWORD Offset = GetOffsetFromHash(szDirectoryName);
	if (Offset)
		return TRUE;

	// ��ȡ�ļ�ӳ���
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ���ǰ��ļ�ͷ
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	// ��ȡ�������丸Ŀ¼��Ӧ��·����
	char szBlockName[MAX_BLOCK_NAME_LEN];
	char szParentPathName[MAX_RELATIVE_PATH_LEN];
	string strDirName = GetFullPathName(szDirectoryName);
	int pos = strDirName.find_last_of('\\');
	lstrcpyA(szBlockName,  string( strDirName.substr( pos + 1, strDirName.length() - pos - 1 ) ).c_str() );
	string strParentDir = strDirName;
	strParentDir.erase(strParentDir.begin() + pos, strParentDir.end());
	lstrcpyA(szParentPathName, string( strParentDir.substr( m_strRootDirectory.length(), strParentDir.length() - m_strRootDirectory.length() ) ).c_str() );

	// ��ȡ��Ŀ¼�Ŀ���Ϣ
	BLOCKINFO bi;
	DWORD dwParentOffset = GetOffsetFromHash(szParentPathName);
	if (!dwParentOffset)
	{
		delete[] pmt;
		return FALSE;
	}
	m_pPackFile->Seek(dwParentOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
	{
		delete[] pmt;
		return FALSE;
	}
	DWORD dwNextOffset = bi.dwNextOffset;	// ��һ�����λ��
	DWORD dwPrevOffset = dwParentOffset;
	int nBlockDepth = bi.u.nBlockDepth;

	if (dwNextOffset == 0) // ��Ŀ¼�����һ����
	{
		// �޸ĸ�Ŀ¼�Ŀ���Ϣ
		bi.dwNextOffset = ph.dwMapOffset;
		m_pPackFile->Seek(dwParentOffset, SEEK_SET);
		m_pPackFile->Write(&bi, sizeof(BLOCKINFO));
	}
	else
	{
		// �Ҹ�Ŀ¼�е����һ���ļ���
		while (1)
		{
			m_pPackFile->Seek(dwNextOffset, SEEK_SET);
			m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
			if (!bi.IsValid()) // ���ǿ���Ϣͷ
			{
				delete[] pmt;
				return FALSE;
			}

			if (bi.dwType == CPackFileSystem::__DIR) // Ŀ¼
			{
				dwPrevOffset = bi.dwPrevOffset;
				// �޸ı������Ϣ
				bi.dwPrevOffset = ph.dwMapOffset;
				m_pPackFile->Seek(dwNextOffset, SEEK_SET);
				m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

				// �޸ĸ�Ŀ¼�����һ���ļ��Ŀ���Ϣ
				m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
				m_pPackFile->Write(&ph.dwMapOffset, sizeof(DWORD));
				break;
			}
			else if (bi.dwType == CPackFileSystem::__FILE) // �ļ�
			{
				dwPrevOffset = dwNextOffset;
				dwNextOffset = bi.dwNextOffset;
				if (bi.dwNextOffset == 0) // ���ļ��������һ����
				{
					bi.dwNextOffset = ph.dwMapOffset;
					m_pPackFile->Seek(dwPrevOffset, SEEK_SET);
					m_pPackFile->Write(&bi, sizeof(BLOCKINFO));
					break;
				}
			}
		}
	}

	// newһ����ṹ�����
	BLOCKINFO* pBlockInfo = new BLOCKINFO();
	ASSERT(pBlockInfo != NULL);

	// ��¼ӳ����Ϣ
	MAPTABLE mt( string( strDirName.substr( m_strRootDirectory.length(), strDirName.length() - m_strRootDirectory.length() ) ).c_str(), ph.dwMapOffset);

	pBlockInfo->bIsValid		= TRUE;
	pBlockInfo->u.nBlockDepth	= nBlockDepth + 1;
	pBlockInfo->SetBlockName(szBlockName);
	pBlockInfo->dwBlockSize		= 0;
	pBlockInfo->dwCompSize		= 0;
	pBlockInfo->dwOffset		= 0;
	pBlockInfo->dwPrevOffset	= dwPrevOffset;
	pBlockInfo->dwNextOffset	= dwNextOffset;
	pBlockInfo->dwType			= CPackFileSystem::__DIR;
	pBlockInfo->dwVersion		= 1;

	// ����д����ļ�
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Write(pBlockInfo, sizeof(BLOCKINFO));
	AddToHash(mt.name, mt.offset);
	m_itCurFile = m_Hash.begin();

	// ��дӳ���
	BOOL bFound = FALSE;
	for (int i = 0; i < ph.dwBlockCount; i ++)
	{
		if (pmt[i].offset == dwPrevOffset) // �ҵ�ǰһ��Ķ�Ӧ��!
		{
			bFound = TRUE;
			m_pPackFile->Seek(ph.dwMapOffset + sizeof(BLOCKINFO), SEEK_SET); // ��λ��
			m_pPackFile->Write(pmt, (i + 1)*sizeof(MAPTABLE)); // дǰһ����ǰ��(����ǰһ��)
			m_pPackFile->Write(&mt, sizeof(MAPTABLE));// д�մ����Ŀ��ӳ���
			m_pPackFile->Write(&pmt[i + 1], (ph.dwBlockCount - i -1)*sizeof(MAPTABLE));// д�´���������ӳ���
			break;
		}
	}
	if (!bFound) // û�ҵ�!����ʧ��
	{
		SAFE_DELETE(pBlockInfo);
		delete[] pmt;
		return NULL;
	}

	// �޸İ��ļ�ͷ
	m_dwBlockCount ++;
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset += sizeof(BLOCKINFO);
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));

	// �ͷŻ���
	SAFE_DELETE(pBlockInfo);
	delete[] pmt;
	return TRUE;
}
//------------------------------------------------------------------------
// ɾ��Ŀ¼
BOOL CPackFileSystem::DeleteDirectory(LPCSTR szDirectoryName)
{
	ASSERT(szDirectoryName != NULL && *szDirectoryName != 0);

	DWORD dwOffset = GetOffsetFromHash(szDirectoryName);
	if (!dwOffset) // ��ϣ���в�����!Ȩ�ҵ���ɾ����
	{
		TraceLn("����CPackFileSystem::DeleteDirectory() �����쳣��Ŀ¼ s% �����ڣ�", szDirectoryName);
		return TRUE;
	}

	BLOCKINFO bi;
	m_pPackFile->Seek(dwOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
		return FALSE;

	// ��¼�����ǰ���ƫ��ֵ
	DWORD dwPrevOffset = bi.dwPrevOffset;
	DWORD dwNextOffset = bi.dwNextOffset;
	int nBlockDepth = bi.u.nBlockDepth;

	if (dwPrevOffset == 0) // ��ɾ����Ŀ¼?û��!!
		return FALSE;

	// ��·��
	string strPathName = szDirectoryName;
	if (*(strPathName.end() - 1) == '\\')
		strPathName.erase(strPathName.end() - 1);

	// ��������Ϊ��Ч
	BOOL bIsValid = FALSE;
	m_pPackFile->Seek(dwOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
	m_pPackFile->Write(&bIsValid, sizeof(BOOL));
	m_dwBlockCount --;
	EraseFromHash(strPathName.c_str());


	if (dwNextOffset == 0) // ���������һ����
	{
		// �޸�ǰһ�����Ϣ
		DWORD dwNext = 0;
		m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
		m_pPackFile->Write(&dwNext, sizeof(DWORD));
	}
	else
	{
		while(1)
		{
			m_pPackFile->Seek(dwNextOffset, SEEK_SET);
			m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
			if (!bi.IsValid()) // ���ǿ���Ϣͷ
				return FALSE;

			if (bi.dwType == CPackFileSystem::__DIR) // Ŀ¼
			{
				if (bi.u.nBlockDepth <= nBlockDepth) // ���Ǳ�Ŀ¼����Ŀ¼
				{
					// �޸ı������Ϣ
					bi.dwPrevOffset = dwPrevOffset;
					m_pPackFile->Seek(dwNextOffset, SEEK_SET);
					m_pPackFile->Write(&bi, sizeof(BLOCKINFO));

					// �޸Ĵ�ɾĿ¼ǰһ�������Ϣ
					m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
					m_pPackFile->Write(&dwNextOffset, sizeof(DWORD));
					break;
				}

				// ���Ŀ¼ɾ��
				m_pPackFile->Seek(dwNextOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
				m_pPackFile->Write(&bIsValid, sizeof(BOOL));
				dwNextOffset = bi.dwNextOffset;
				strPathName += "\\";
				strPathName += bi.cbName;
				m_dwBlockCount --;
				EraseFromHash(strPathName.c_str());

				if (bi.dwNextOffset == 0) // ��Ŀ¼�������һ����
				{
					if (dwPrevOffset == 0) // û��Ҫд��!
						break;
					m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
					m_pPackFile->Write(&dwNextOffset, sizeof(DWORD));
					break;
				}				
			}
			else if (bi.dwType == CPackFileSystem::__FILE) // �ļ�
			{
				// ����ļ�ɾ��
				m_pPackFile->Seek(dwNextOffset + offsetof(BLOCKINFO, bIsValid), SEEK_SET);
				m_pPackFile->Write(&bIsValid, sizeof(BOOL));
				dwNextOffset = bi.dwNextOffset;
				string str = strPathName;
				str += "\\";
				str += bi.cbName;
				m_dwBlockCount --;
				EraseFromHash(str.c_str());

				if (bi.dwNextOffset == 0) // ���ļ��������һ����
				{
					m_pPackFile->Seek(dwPrevOffset + offsetof(BLOCKINFO, dwNextOffset), SEEK_SET);
					m_pPackFile->Write(&dwNextOffset, sizeof(DWORD));
					break;
				}
			}

		}
	}

	// ɾ��ӳ���Ķ�Ӧ��
	PACKHEADER ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ���ǰ��ļ�ͷ
		return FALSE;
	MAPTABLE* pmt = new MAPTABLE[ph.dwBlockCount];
	m_pPackFile->Seek(ph.dwMapOffset, SEEK_SET);
	m_pPackFile->Read(pmt, sizeof(MAPTABLE)*ph.dwBlockCount);

	BOOL bFound = FALSE;
	if (dwPrevOffset == 0) // ���ɾ�����Ƕ���Ŀ¼
	{
		m_dwBlockCount = 0;
		m_Hash.clear();
	}
	else
	{
		for (int i = 0; i < ph.dwBlockCount; i ++)
		{
			if (pmt[i].offset == dwPrevOffset) // �ҵ���ɾĿ¼��ǰһ��!
			{
				bFound = TRUE;
				m_pPackFile->Seek(ph.dwMapOffset + (i + 1)*sizeof(MAPTABLE), SEEK_SET);
				// ������ûɾ�Ŀ��Ƶ���ɾ��λ��(�������:����Ķ�ɾ��)
				m_pPackFile->Write(&pmt[i + 1 + ph.dwBlockCount - m_dwBlockCount], 
					(m_dwBlockCount - i -1)*sizeof(MAPTABLE)); // ������������ӳ�����û��ɾ��!!
				break;
			}
		}
		if (!bFound) // û�ҵ�!!
		{
			SAFE_DELETE(pmt);
			return FALSE;
		}
	}
	SAFE_DELETE(pmt);

	// �޸İ��ļ�ͷ
	ph.dwBlockCount = m_dwBlockCount;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Write(&ph, sizeof(PACKHEADER));
	TraceLn("DeleteDirectory: %s", szDirectoryName);
	return TRUE;
}
//------------------------------------------------------------------------
// 1����Ŀ¼̬�ļ�ϵͳ���Ƴɰ�̬�ļ�ϵͳ���򷴹���
// 2������ǰ�ļ���ϵͳ���Ƶ���һ���ļ���ϵͳ��
// ���ڰ��ļ�ϵͳ�У�������ɾʱ���ֺܶ�ն������Ƶ����ļ���ϵͳ���Խ���������
BOOL CPackFileSystem::CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags/* = 0*/, PACKPROC lpfnPackProc/* = NULL*/)
{
	ASSERT(szNewFileSystemName != NULL && *szNewFileSystemName != 0);

	if (nFlags == 0)
		return PackToDir(szNewFileSystemName, lpfnPackProc);
	else // Pack to Pack
		return PackToPack(szNewFileSystemName, lpfnPackProc);

	return FALSE;
}
//------------------------------------------------------------------------
// �ͷ�
BOOL CPackFileSystem::Release()
{
#ifdef _DEBUG
	char szbuf[256];
	wsprintfA(szbuf, "�ͷ��ļ�ϵͳ %s ���......\r\n", m_pPackFile->GetFilePath());
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
BOOL CPackFileSystem::PackToDir(LPCSTR szDirectoryName, PACKPROC lpfnPackProc/* = NULL*/)
{
	// �������ļ�ͷ
	PACKHEADER	ph;
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid()) // ���ǰ��ļ�ͷ
		return FALSE;
	m_dwBlockCount = ph.dwBlockCount;

	// �ļ�ָ��ƫ��
	m_dwFilePos = 0;
	m_dwFilePos += sizeof(PACKHEADER);

	DWORD dwBlockCount = 0;
	int nBlockDepth = -1;
	string curDir = szDirectoryName;

	// �Ƿ����еĿ鶼������
	while (dwBlockCount < m_dwBlockCount)
	{
		// ��������Ϣ
		BLOCKINFO bi;
		m_pPackFile->Seek(m_dwFilePos, SEEK_SET);
		m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
		if (!bi.IsValid()) // ���ǿ���Ϣͷ
		{
			TraceLn("���󣺴���Ŀ��ļ���Ϣͷ");
			return FALSE;
		}

		dwBlockCount ++;
		m_dwFilePos = bi.dwNextOffset;

		// ��Ч��
		if (bi.bIsValid) // û�б�ʶΪɾ��״̬
		{
			// ȷ����ľ�����
			if (*(curDir.end() - 1) != '\\') // ��������"\"
				curDir += '\\';

			// �����
			if (bi.dwType == CPackFileSystem::__DIR) // ��Ŀ¼��
			{
				// �Ƚ�Ŀ¼�����
				if (bi.u.nBlockDepth > nBlockDepth)
					nBlockDepth = bi.u.nBlockDepth;
				else
				{
					int pos = curDir.length();
					for( int i = 0; i < (nBlockDepth - bi.u.nBlockDepth + 2); i ++)
					{
						pos = curDir.rfind('\\', pos - 1);
					}
					curDir.erase(curDir.begin() + pos, 
						curDir.begin() + curDir.length() - 1);
				}
				nBlockDepth = bi.u.nBlockDepth;
				// �����µĵ�ǰĿ¼
				curDir += bi.cbName;

				if (!DirectoryExist(curDir.c_str()))
				{
					SECURITY_ATTRIBUTES sa;
					sa.nLength=sizeof(SECURITY_ATTRIBUTES);
					sa.lpSecurityDescriptor = NULL;
					sa.bInheritHandle = 0;
					if (!::CreateDirectoryA(curDir.c_str(), &sa))
					{
						TraceLn("���� CreateDirectory() �����쳣��GetLastError() = %d", ::GetLastError());
						return FALSE;
					}
				}
			}
			else // ���ļ���
			{
				// ����·���ļ���
				string curFile = curDir;
				curFile += bi.cbName;

				CStdFileObject* pBlockObject = NULL;
				if (bi.u.bCompress) // ��ѹ��
				{
					//....
				}
				else // û��ѹ��
				{
					// ����һ�����ļ�,����������д����
					pBlockObject = new CStdFileObject();
					pBlockObject->SetFilePath(curFile.c_str());
					if (!pBlockObject->Open("wb")) // ���������
						return FALSE;

					// �����ļ�����
					char* buf = new char[bi.dwBlockSize];

					// ���ļ����ݶ��뻺��
					m_pPackFile->Seek(bi.dwOffset, SEEK_SET);
					m_pPackFile->Read(buf, bi.dwBlockSize);

					// ����������д���´������ļ���
					pBlockObject->Write(buf, bi.dwBlockSize);

					// �ر��ļ�
					pBlockObject->Close();

					// �ͷŻ���ռ�
					delete[] buf;
					pBlockObject->Release();
					pBlockObject = NULL;
				}
			}
		}
		
		// �ص�
		if (lpfnPackProc)
			lpfnPackProc(1);
	}

	m_pPackFile->SeekToBegin();

	return TRUE;
}
//------------------------------------------------------------------------
#if 0
BOOL CPackFileSystem::HandleBlock(LPCSTR szCurDir, PACKPROC lpfnPackProc/* = NULL*/)
{
	static DWORD dwBlockCount = 0;
	static int nBlockDepth = -1;

	// �Ƿ����еĿ鶼������
	if (dwBlockCount >= m_dwBlockCount)
	{
		dwBlockCount = 0;
		nBlockDepth = -1;
		return TRUE;
	}

	// ��������Ϣ
	BLOCKINFO bi;
	m_pPackFile->Seek(m_dwFilePos, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid()) // ���ǿ���Ϣͷ
		return FALSE;
	dwBlockCount ++;
	m_dwFilePos = bi.dwNextOffset;

	// ��Ч��
	if (bi.bIsValid == FALSE)
		return TRUE;

	// ȷ����ľ�����
	string strBlockName = szCurDir;
	if (*(strBlockName.end() - 1) != '\\') // ��������"\"
		strBlockName += '\\';

	// �����
	if (bi.dwType == CPackFileSystem::__DIR) // ��Ŀ¼��
	{
		// �Ƚ�Ŀ¼�����
		if (bi.u.nBlockDepth > nBlockDepth)
			nBlockDepth = bi.u.nBlockDepth;
		else
		{
			int pos = strBlockName.length();
			for( int i = 0; i < (nBlockDepth - bi.u.nBlockDepth + 2); i ++)
			{
				pos = strBlockName.rfind('\\', pos - 1);
			}
			strBlockName.erase(strBlockName.begin() + pos, 
				strBlockName.begin() + strBlockName.length() - 1);
		}
		nBlockDepth = bi.u.nBlockDepth;
		// �����µĵ�ǰĿ¼
		strBlockName += bi.cbName;

		if (!DirectoryExist(strBlockName.c_str()))
		{
			SECURITY_ATTRIBUTES sa;
			sa.nLength=sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = 0;
			if (!::CreateDirectory(strBlockName.c_str(), &sa))
			{
				TraceLn("ERROR: Api function CreateDirectory() catch error,GetLastError() = %d", ::GetLastError());
				return FALSE;
			}
		}

		// �ص�
		if (lpfnPackProc)
			lpfnPackProc(1);

		//DebugMsgLn("CreateDirectory: %s", strBlockName.c_str());
		HandleBlock(strBlockName.c_str(), lpfnPackProc);
		return TRUE;
	}
	else // ���ļ���
	{
		// ����·���ļ���
		strBlockName += bi.cbName;

		CStdFileObject* pBlockObject = NULL;
		if (bi.u.bCompress) // ��ѹ��
		{
			//....
		}
		else // û��ѹ��
		{
			// ����һ�����ļ�,����������д����
			pBlockObject = new CStdFileObject();
			pBlockObject->SetFilePath(strBlockName.c_str());
			if (!pBlockObject->Open("wb")) // ���������
				return FALSE;

			// �����ļ�����
			char* buf = new char[bi.dwBlockSize];

			// ���ļ����ݶ��뻺��
			m_pPackFile->Seek(bi.dwOffset, SEEK_SET);
			m_pPackFile->Read(buf, bi.dwBlockSize);

			// ����������д���´������ļ���
			pBlockObject->Write(buf, bi.dwBlockSize);

			// �ر��ļ�
			pBlockObject->Close();

			// �ͷŻ���ռ�
			delete[] buf;
			pBlockObject->Release();
			pBlockObject = NULL;
		}

		// �ص�
		if (lpfnPackProc)
			lpfnPackProc(1);

		//DebugMsgLn("CreateFile     : %s", strBlockName.c_str());
		HandleBlock(szCurDir, lpfnPackProc);
		return TRUE;
	}


	return TRUE;
}
#endif
//------------------------------------------------------------------------
BOOL CPackFileSystem::PackToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc/* = NULL*/)
{
	PACKHEADER ph;
	BLOCKINFO bi;
	DWORD dwFilePos = 0;
	vector<MAPTABLE> MapTable;
	DWORD dwPrevOffset = 0;
	DWORD dwCurrOffset = 0;
	char* pFileBuffer = NULL;
	DWORD dwBlockDepth = 0;

	// �����µİ��ļ�����
	CStdFileObject* pNewPackFile = new CStdFileObject;
	pNewPackFile->SetFilePath(GetFullPathName(szPackFileName));
	pNewPackFile->Open("wb");

	// �������ļ�ͷ
	m_pPackFile->SeekToBegin();
	m_pPackFile->Read(&ph, sizeof(PACKHEADER));
	if (!ph.IsValid())
		return FALSE;
	// д��ͷ
	ph.dwReserved1 = 1; // ����Ϊ1�������Ϊ0���������Լ���Ƿ���������˳�����
	pNewPackFile->Write(&ph, sizeof(PACKHEADER));
	pNewPackFile->Flush();

	dwFilePos += sizeof(PACKHEADER);
	dwCurrOffset = ph.dwBlockOffset;

	string strDir = "";
	string strFileName;

	// д����Ŀ¼
	m_pPackFile->Seek(dwCurrOffset, SEEK_SET);
	m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
	if (!bi.IsValid())
		return FALSE;
	dwCurrOffset = bi.dwNextOffset;
	// �޸Ŀ���Ϣ
	bi.dwPrevOffset = dwPrevOffset;

	bi.dwNextOffset = dwFilePos + sizeof(BLOCKINFO);
	pNewPackFile->Write(&bi, sizeof(BLOCKINFO));	// д����Ϣ
	dwPrevOffset = dwFilePos;

	MAPTABLE mt(bi.cbName, dwFilePos);
	MapTable.push_back(mt);
	dwFilePos += sizeof(BLOCKINFO);


	// д���еĿ�
	while (dwCurrOffset)
	{
		// ��������Ϣ
		m_pPackFile->Seek(dwCurrOffset, SEEK_SET);
		m_pPackFile->Read(&bi, sizeof(BLOCKINFO));
		if (!bi.IsValid())
			return FALSE;
		dwCurrOffset = bi.dwNextOffset;
		// ���ļ�������ļ�����
		if (bi.dwType == CPackFileSystem::__FILE)
		{
			pFileBuffer = new char[bi.dwCompSize];
			m_pPackFile->Seek(bi.dwOffset);
			m_pPackFile->Read(pFileBuffer, bi.dwCompSize);
		}

		// �޸Ŀ���Ϣ
		bi.dwPrevOffset = dwPrevOffset;
		if (bi.dwType == CPackFileSystem::__FILE)
		{
			bi.dwOffset = dwFilePos + sizeof(BLOCKINFO);
			bi.dwNextOffset = dwFilePos + sizeof(BLOCKINFO) + bi.dwCompSize;
			pNewPackFile->Write(&bi, sizeof(BLOCKINFO));		// д����Ϣ
			pNewPackFile->Write(pFileBuffer, bi.dwCompSize);	// д�ļ�
			SAFE_DELETE(pFileBuffer);
			dwPrevOffset = dwFilePos;

			// �ص�
			if (lpfnPackProc)
				lpfnPackProc(1);

			strFileName = strDir;
			strFileName += bi.cbName;
			MAPTABLE mt(strFileName.c_str(), dwFilePos);
			MapTable.push_back(mt);

			dwFilePos += sizeof(BLOCKINFO) + bi.dwCompSize;
		}
		else if (bi.dwType == CPackFileSystem::__DIR)
		{
			bi.dwNextOffset = dwFilePos + sizeof(BLOCKINFO);
			pNewPackFile->Write(&bi, sizeof(BLOCKINFO));	// д����Ϣ
			dwPrevOffset = dwFilePos;

			if (bi.u.nBlockDepth > dwBlockDepth)
			{
				strDir += bi.cbName;
				strDir += "\\";
				dwBlockDepth = bi.u.nBlockDepth;
			}
			else
			{
				int pos = strDir.length();
				for( int i = 0; i < (dwBlockDepth - bi.u.nBlockDepth + 2); i ++)
				{
					pos = strDir.rfind('\\', pos - 1); // ���˵�����ʱ, pos = -1
				}
				strDir.erase(strDir.begin() + pos + 1, strDir.end());
				strDir += bi.cbName;
				strDir += "\\";
				dwBlockDepth = bi.u.nBlockDepth;
			}

			// �ص�
			if (lpfnPackProc)
				lpfnPackProc(1);
			
			string str = strDir;
			if (*(str.end() - 1) == '\\')
				str.erase(str.end() - 1);
			MAPTABLE mt(str.c_str(), dwFilePos);
			dwFilePos += sizeof(BLOCKINFO);
			MapTable.push_back(mt);
		}
	}

	// д�ļ�ӳ���
	pNewPackFile->Write(&MapTable[0], MapTable.size()*sizeof(MAPTABLE));

	// �޸İ��ļ�ͷ��ĳЩ����
	//DebugMsgLn("Block count: %d", m_dwBlockCount);
	ph.dwBlockCount = m_dwBlockCount;
	ph.dwMapOffset = dwFilePos;
	ph.dwReserved1 = 0;
	pNewPackFile->SeekToBegin();
	pNewPackFile->Write(&ph, sizeof(PACKHEADER));

	// �ļ��ر�
	pNewPackFile->Close();
	pNewPackFile->Release();

	return TRUE;
}
//------------------------------------------------------------------------
// ������·���ľ���·��(��������Ǿ���·��,���Թ�)
inline const char* CPackFileSystem::GetFullPathName(LPCSTR szRelativePathName)
{
	if (szRelativePathName[1] == ':') // ������Ǿ���·��
		return szRelativePathName;

	m_strFullPathName = m_strRootDirectory;
	m_strFullPathName += szRelativePathName;
	return m_strFullPathName.c_str();
}
//------------------------------------------------------------------------
void CPackFileSystem::AddToHash(LPCSTR szName, DWORD dwOffset)
{
	char name[MAX_RELATIVE_PATH_LEN];
	lstrcpyA(name, szName);
	ToLower(name);
	m_Hash[name] = dwOffset;
}
//------------------------------------------------------------------------
DWORD CPackFileSystem::GetOffsetFromHash(LPCSTR szName)
{
	char name[MAX_RELATIVE_PATH_LEN];
	lstrcpyA(name, szName);
	ToLower(name);
	return m_Hash[name];
}
//------------------------------------------------------------------------
void CPackFileSystem::EraseFromHash(LPCSTR szName)
{
	char name[MAX_RELATIVE_PATH_LEN];
	lstrcpyA(name, szName);
	ToLower(name);
	m_Hash.erase(name);
}
//------------------------------------------------------------------------
void CPackFileSystem::ToLower(LPSTR szString)
{
	char* st = szString;
	while (*st != 0)
	{
		char val = *st;

		if ((val >= 'A') && (val <='Z')) 
			*st =(char)(val | 32);
		else if (val == '/')
			*st = (char)'\\';

		st ++;
	}
}
//------------------------------------------------------------------------
// �ж�Ŀ¼�Ƿ����
BOOL CPackFileSystem::DirectoryExist(LPCSTR szDirectoryName)
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
BOOL CPackFileSystem::MakeDirectory(LPCSTR szDirectoryName)
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
DWORD CPackFileSystem::GetLength()
{
	return m_pPackFile->GetLength();
}
//------------------------------------------------------------------------