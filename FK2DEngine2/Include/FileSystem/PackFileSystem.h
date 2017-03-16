/**
*	created:		2012-6-16   1:45
*	filename: 		PackFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CPackFileSystem  : public IFileSystem
{
public:
	enum {__DIR = 0, __FILE};
private:
	string			m_strRootDirectory;	// �ļ�ϵͳ����Ŀ¼��
	CStdFileObject*	m_pPackFile;		// ���ļ�����
	string			m_strFullPathName;	// ���ڻ����ļ�(Ŀ¼)�ľ���·����
	DWORD			m_dwLastBlockOffset;// ���һ������Ϣͷ��ƫ��(ͨ�������Խ����һ�����dwNextOffset��Ϊ0,��������DirToPack()����)
	DWORD			m_dwFilePos;		// ��ʶĿǰд���������е��ļ����ĸ�λ��(�ֽ�)
	DWORD			m_dwBlockCount;		// �������ļ��Ŀ���(Ŀ¼���ļ�������)
	vector<MAPTABLE>	m_MapTable;		// ѹ��Ŀ¼ϵͳʱ���ڼ�¼����ļ������ļ���ƫ��ֵ
	map<string, DWORD> m_Hash;			// ӳ���hash�ṹ,����ѹ���ļ�ʱ�Զ�����
	map<string, DWORD>::iterator m_itCurFile;

public:
	CPackFileSystem();
	virtual ~CPackFileSystem();

	// �����ļ�
	IFileObject* CreateFile(LPCSTR szFileName, char* pData = NULL, int nSize = 0, UINT nStyle = CFS_NEW);
	// ���ĳ���ļ�����(�����򿪶�Ӧ�ļ�!)
	IFileObject* GetFile(LPCSTR szFileName);
	IFileObject* GetIndexFile(LPCSTR szFileName, DWORD dwCompSize);
	IFileObject* GetFirstFile();
	IFileObject* GetNextFile();
	DWORD GetLength();
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

	BOOL Init(LPCSTR szFileSystemName);

	BOOL PackToDir(LPCSTR szDirectoryName, PACKPROC lpfnPackProc = NULL);
	BOOL PackToPack(LPCSTR szPackFileName, PACKPROC lpfnPackProc = NULL);
	BOOL HandleBlock(LPCSTR szCurDir, PACKPROC lpfnPackProc = NULL);
	void ToLower(LPSTR szString);
	void AddToHash(LPCSTR szName, DWORD dwOffset);
	DWORD GetOffsetFromHash(LPCSTR szName);
	void EraseFromHash(LPCSTR szName);
	// �ж�Ŀ¼�Ƿ����
	BOOL DirectoryExist(LPCSTR szDirectoryName);
	BOOL MakeDirectory(LPCSTR szDirectoryName);

	inline const char* GetFullPathName(LPCSTR szRelativePathName);
private:
	BOOL UpdateMapTable();
};
//------------------------------------------------------------------------
