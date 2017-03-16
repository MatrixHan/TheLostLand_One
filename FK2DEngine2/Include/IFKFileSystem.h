/**
*	created:		2012-6-15   22:04
*	filename: 		IFKFileSystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "FKSingleton.h"
//------------------------------------------------------------------------
struct ITrace;
//------------------------------------------------------------------------
// �ļ�ϵͳ�汾��
#define VERSION_QUERY_FILE_SYSTEM		1
// �ļ�ϵͳ����(File System Style)�궨��
#define FSS_DIRECTORY			0	// �ļ�ϵͳ��Ŀ¼̬��ʽ����
#define FSS_PACKAGE				1	// �ļ�ϵͳ�԰�̬��ʽ����
// �����ļ�������(CreateFile Style)�궨��
#define CFS_NEW					0	// �½�һ���ļ������������
#define CFS_UPDATE				1	// ����һ���ļ����ļ��������
// �ַ������ȶ���
#define MAX_BLOCK_NAME_LEN		32  // ����������
#define MAX_RELATIVE_PATH_LEN	64  // ������·������
// �ļ�ϵͳѹ���ͽ���ص�����,dwFileCountsΪÿ�λص�ʱ�����˵��ļ���Ŀ
typedef void (CALLBACK* PACKPROC)(DWORD dwFileCounts);

//------------------------------------------------------------------------
// �ļ�����ӿڡ�����������MFC��CFile���CStdioFile
struct IFileObject
{
	/* �й��ļ���ģʽ����Ϣ: 
	"r"		��    ,�ļ���������ʧ��
	"w"		д    ,�ļ��������򴴽�,�������������
	"a"		���  ,�ļ��������򴴽�
	"r+"	��д  ,�ļ���������ʧ��
	"w+"	��д  ,�ļ��������򴴽�,�������������
	"a+"	�����,�ļ��������򴴽�
	"t"		�ı���ʽ
	"b"		�����Ʒ�ʽ
	*/

	// �ļ��򿪹ر�(Ĭ����ֻ������������ʽ��)
	virtual BOOL Open(const char* lpszMode = "rb") = 0;
	virtual BOOL Close() = 0;
	
	// ���Ķ�д
	// ���ڰ��ļ��е��ļ�����ܾ�����Խ��Ķ�д
	virtual UINT Read(void* lpBuf, UINT nCount) = 0;
	virtual BOOL Write(const void* lpBuf, UINT nCount) = 0;

	// �ı��ļ�����(��CStdioFile��ĳ�Ա��������)
	// ���ڰ��ļ��е��ļ�����ܾ�����Խ��Ķ�д
	// �������ļ�β��,��������ֵΪNULL
	virtual char* ReadString(char* lpsz, UINT nMax) = 0;
	virtual BOOL WriteString(const char* lpsz) = 0;

	// �ļ�ָ��λ�ö�λ
	// ���ļ��Ĳ����뵥���ļ�����
	// �ο�: SEEK_SET �ļ�ͷ; SEEK_CUR �ļ���ǰλ��; SEEK_END �ļ�β
	virtual BOOL Seek(LONG lOff, UINT nFrom = SEEK_SET) = 0;
	virtual BOOL SeekToBegin() = 0;
	virtual BOOL SeekToEnd() = 0;
	virtual int	GetPosition() = 0;

	// �ļ��������
	// ���ļ���֧��
	virtual BOOL Flush() = 0;

	// ��ȡ�ļ�����(�ļ�û��ʱҲ�ܻ�ȡ����)
	// ����ǰ��ļ�,����ָѹ����ĳ���
	virtual DWORD GetLength() = 0;
	virtual BOOL SetLength(DWORD dwNewLen) = 0;

	// ����ļ���,����"filetitle.ext"
	virtual const char* GetFileName() = 0;
	// ����ļ�������,����"filetitle"
	virtual BOOL GetFileTitle(char* buf) = 0;
	// ����ļ���·����,����"d:\dxGame\filetitle.ext"
	virtual const char* GetFilePath() = 0;

	// �ļ��Ƿ����
	// ע: ���ļ���֧��,��̬ʱ��ͨ��IFileSystem->GetFile(...)���ж��Ƿ����
	virtual BOOL IsExist() = 0;
	// �ͷ�(��ס:��������������)
	virtual BOOL Release() = 0;
};

//------------------------------------------------------------------------
// �ļ�ϵͳ�ӿڣ����ļ�������в���
struct IFileSystem
{
	// �����ļ�
	// ����ļ�����,����ʧ��,����NULL,��Trace����;
	// ����ֵ: �ɹ������ļ�����;ʧ�ܷ���NULL
	// ע: 1���û����Լ��ͷŻ�õ��ļ�����IFileObject->Release()
	//	   2���ļ����������·��
	//	   3������pData��nSize���������ڰ�̬�д����ļ�;��Ŀ¼̬ʱ����
	//	   4��Ŀ¼̬ʱ,�ò����½�һ���ļ�(û�д�,�û��Լ����ô�ģʽ����!!)
	virtual IFileObject* CreateFile(const char* szFileName, char* pData = NULL, int nSize = 0, UINT nStyle = CFS_NEW) = 0;

	// ���ĳ���ļ�����(�����򿪶�Ӧ�ļ�!)
	// ����ֵ: �ɹ������ļ�����;ʧ�ܷ���NULL
	// �ļ����������·��
	virtual IFileObject* GetFile(const char* szFileName) = 0;

	// ����Ŀ¼(Ŀ¼̬֧�ֶ༶����,��̬��֧��)
	// �ɹ�����TRUE;ʧ�ܷ���FALSE
	// Ŀ¼���������·��
	virtual BOOL CreateDirectory(const char* szDirectoryName) = 0;

	// ɾ��ָ�����ļ�
	// �����ɾ���ļ�������,�������˲���(Ȩ��ִ����ɾ��),����TRUE,��Trace����;
	// ����ֵ: �ɹ�����TRUE;ʧ�ܷ���FALSE
	// �ļ����������·��;���ļ��е�ɾ�����������Ǳ�ʶΪ��Ч����
	virtual BOOL DeleteFile(const char* szFileName) = 0;

	// ɾ��Ŀ¼(�����༶��Ŀ¼����ļ�)
	// �����ɾ��Ŀ¼������,�������˲���(Ȩ��ִ����ɾ��),����TRUE,��Trace����;
	// ����ֵ: �ɹ�����TRUE;ʧ�ܷ���FALSE
	// Ŀ¼���������·��;���ļ��е�ɾ�����������Ǳ�ʶΪ��Ч����
	virtual BOOL DeleteDirectory(const char* szDirectoryName) = 0;

	// �ļ�(Ŀ¼)������
	// ������������ļ�������,�����ʧ��,����FALSE,��Trace����;
	// ����ֵ: �ɹ�����TRUE;ʧ�ܷ���FALSE
	// ע: 1�������޸�����,������ı�·��; 2���������·��
	virtual BOOL Rename(const char* szOldFileName,const char* szNewFileName) = 0;
	
	// 1����Ŀ¼̬�ļ�ϵͳ���Ƴɰ�̬�ļ�ϵͳ���򷴹���(nFlags == 0)
	// 2������ǰ�ļ���ϵͳ���Ƶ���һ���ļ���ϵͳ��(nFlags != 0)
	// ���ڰ��ļ�ϵͳ�У�������ɾʱ���ֺܶ�ն������Ƶ����ļ���ϵͳ���Խ���������
	// ע: ����Ĳ������þ���·��!!!
	virtual BOOL CopyToFileSystem(const char* szNewFileSystemName, int nFlags = 0, PACKPROC lpfnPackProC = NULL) = 0;

	// �ͷ�
	// ע�������ͷ��Լ�,����������������IFileObject
	virtual BOOL Release() = 0;
	
	virtual IFileObject* GetFirstFile() = 0;
	virtual IFileObject* GetNextFile() = 0;
	virtual DWORD GetLength() = 0;
	virtual IFileObject* GetIndexFile(const char* szFileName, DWORD dwCompSize) = 0;
};


//------------------------------------------------------------------------
// �ļ�ϵͳ������
class CIFileSystemHelper : public TSingletonClass< CIFileSystemHelper >
{
public:
	CIFileSystemHelper()
	{
	}
	~CIFileSystemHelper()
	{
	}
	// ������� : CreateFileSystem
	// ������
	// szFileSystemName : 	����Ŀ¼̬ϵͳ,����ĳ���ļ����ڵ�Ŀ¼����e:\dxGame\data\bmp
	//						���ڰ�̬�ļ�ϵͳ,���Ǹ����ļ�
	// dwStyle			: 	ָ���Ǵ���Ŀ¼̬�ļ�ϵͳ���ǰ�̬�ļ�ϵͳ��������ָ������ѡ��
	BOOL CreateFileSystem(DWORD dwExpectVersion, const char* szFileSystemName, DWORD dwStyle, ITrace* pTrace,
		IFileSystem** ppFileSystem,	void* Reserved);
	BOOL CreateFileObject(DWORD dwExpectVersion, const char* szFileName, ITrace* pTrace,
		IFileObject** ppFileObject);
	BOOL Create(IFileSystem** ppFileSystem, const char* szFileSystemName, DWORD dwStyle, ITrace* pTrace,	void* Reserved = NULL)
	{
		if(!CreateFileSystem(VERSION_QUERY_FILE_SYSTEM, szFileSystemName, dwStyle, pTrace, ppFileSystem, Reserved))
		{
			return false;
		}
		return ppFileSystem != NULL;
	}
};
//------------------------------------------------------------------------