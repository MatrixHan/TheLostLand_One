/**
*	created:		2012-6-16   0:12
*	filename: 		StdFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CStdFileSystem  : public IFileSystem
{
public:
	enum {__DIR = 0, __FILE};
private:
	string			m_strRootDirectory;		// �ļ�ϵͳ����Ŀ¼��
	CStdFileObject*	m_pPackFile;			// ���ļ�����
	string			m_strFullPathName;		// ���ڻ����ļ�(Ŀ¼)�ľ���·����
	DWORD			m_dwLastBlockOffset;	// ���һ������Ϣͷ��ƫ��(ͨ�������Խ����һ�����dwNextOffset��Ϊ0,��������DirToPack()����)

	DWORD			m_dwFilePos;			// ��ʶĿǰд���������е��ļ����ĸ�λ��(�ֽ�)
	DWORD			m_dwBlockCount;			// �������ļ��Ŀ���(Ŀ¼���ļ�������)

	vector<MAPTABLE>	m_MapTable;			// ѹ��Ŀ¼ϵͳʱ���ڼ�¼����ļ������ļ���ƫ��ֵ

public:
	CStdFileSystem();
	virtual ~CStdFileSystem();

	// IFileSystem
	// �����ļ�
	IFileObject* CreateFile(LPCSTR szFileName, char* pData = NULL, int nSize = 0, UINT nStyle = CFS_NEW);
	// ���ĳ���ļ�����(�����򿪶�Ӧ�ļ�!)
	IFileObject* GetFile(LPCSTR szFileName);
	virtual IFileObject* GetFirstFile() {return NULL;};
	virtual IFileObject* GetNextFile() {return NULL;};
	virtual IFileObject* GetIndexFile(LPCSTR szFileName, DWORD dwCompSize) {return NULL;};
	virtual DWORD GetLength() {return 0;};
	// ����Ŀ¼
	BOOL CreateDirectory(LPCSTR szDirectoryName);
	// ɾ��ָ�����ļ�
	BOOL DeleteFile(LPCSTR szFileName);
	// ɾ��Ŀ¼
	BOOL DeleteDirectory(LPCSTR szDirectoryName);
	// �ļ�������
	BOOL Rename(LPCSTR szOldFileName,LPCSTR szNewFileName);
	// 1����Ŀ¼̬�ļ�ϵͳ���Ƴɰ�̬�ļ�ϵͳ���򷴹���
	// 2������ǰ�ļ���ϵͳ���Ƶ���һ���ļ���ϵͳ��
	// ���ڰ��ļ�ϵͳ�У�������ɾʱ���ֺܶ�ն������Ƶ����ļ���ϵͳ���Խ���������
	BOOL CopyToFileSystem(LPCSTR szNewFileSystemName, int nFlags = 0, PACKPROC lpfnPackProc = NULL);
	// �ͷ�
	BOOL Release();

	// Other
	BOOL Init(LPCSTR szFileSystemName);
	BOOL SetRootDir(LPCSTR szPath);

	BOOL DirToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc = NULL);

	BOOL BrowseDir(LPCSTR szDir, PACKPROC lpfnPackProc = NULL);

	BOOL DirectoryExist(LPCSTR szDirectoryName);
	BOOL MakeDirectory(LPCSTR szDirectoryName);
	BOOL DeleteDirectoryEx(LPCSTR szDirectoryName);

	inline const char* GetFullPathName(LPCSTR szRelativePathName);
};
//------------------------------------------------------------------------