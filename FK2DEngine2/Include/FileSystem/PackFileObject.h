/**
*	created:		2012-6-16   1:39
*	filename: 		PackFileObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CPackFileObject  : public IFileObject
{
private:
	FILE*	m_pFile;
	string	m_strFileName; 
	DWORD	m_dwOffset;		// ���ļ��Ŀ���Ϣ�ڰ��ļ��е�ƫ����
	DWORD	m_dwFileLength; // ���ļ����ȣ�����ǰ��ļ�����ָѹ������ļ��ߴ�
public:
	CPackFileObject();
	virtual ~CPackFileObject();

	BOOL Open(LPCSTR lpszMode = "rb");
	BOOL Close();

	UINT Read(void* lpBuf, UINT nCount);
	BOOL Write(const void* lpBuf, UINT nCount);

	LPSTR ReadString(LPSTR lpsz, UINT nMax);
	BOOL WriteString(LPCSTR lpsz);

	BOOL Seek(LONG lOff, UINT nFrom = SEEK_SET);
	BOOL SeekToBegin();
	BOOL SeekToEnd();
	int GetPosition();

	BOOL Flush();

	DWORD GetLength();
	BOOL SetLength(DWORD dwNewLen);

	LPCSTR GetFileName();
	BOOL GetFileTitle(char* buf);
	LPCSTR GetFilePath();

	BOOL IsExist();
	BOOL Release();

	BOOL SetFilePath(LPCSTR lpszNewName);
	BOOL IsOpen();
	BOOL SetFileInfo(FILE* pFile, DWORD dwOffset, DWORD dwFileLength);
	FILE* GetHandle();
};
//------------------------------------------------------------------------