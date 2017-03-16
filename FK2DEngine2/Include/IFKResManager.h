/**
*	created:		2012-6-18   18:55
*	filename: 		IFKResManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
#include "FKSingleton.h"
//------------------------------------------------------------------------
// ��Դ�������汾
#define VERSION_QUERY_RESOURCE_MANAGER		1
// ������Դ�ű���󳤶�
#define	MAX_SCRIPT_LINE_LEN					2048
//------------------------------------------------------------------------
// ��Դ����ӿ���
struct IResScript;
struct IFileObject;
struct IResObject
{
public:
	// ���ö�������ֺ�����ģ��
	virtual void SetObjectInfo(LPCSTR szObjectName = NULL, LPCSTR szTypeName = NULL) = 0;
	// ��ȡ������
	virtual LPCSTR GetObjectName() = 0;
	// ��ȡ��������ģ����
	virtual LPCSTR GetTypeName() = 0;
	// ��ȡ����ID(ϵͳ�Զ�������Root�����IDΪ0,�������������0)
	virtual UINT GetID() = 0;
	// ���(��������ڽ���ɾ������������ԣ���������������)
	virtual void Empty() = 0;
	// �ͷŴ˶���(���������Ӷ���)��һ�㲻Ҫ���ô˷�����Ŀǰ�˷�����ҪӦ�����ļ��ϴ���������
	virtual BOOL Release() = 0;
	// ��ȡ�����������Ŀ
	virtual int  GetPropertyCount() = 0;
	// �������Ż�ȡ�����������
	virtual LPCSTR GetPropertyName(int nIndex) = 0;
	// �������Ż�ȡ���������ֵ
	virtual LPCSTR GetPropertyValue(int nIndex) = 0;

	// ��ȡ��Դ�ű���ָ��ID�Ķ���
	virtual IResObject* GetResObject(UINT uID) = 0;

	// ��ȡ������ID
	virtual UINT GetParentID() = 0;
	// ��ȡ������(����Root����)
	virtual IResObject* GetParentObject() = 0;

	// �����µ��Ӷ���
	virtual IResObject* CreateSubObject(int nIndex = -1) = 0;
	// ��ȡ�Ӷ�����Ŀ
	virtual int GetSubObjectCount() = 0;
	// ͨ����������ȡ�Ӷ���(��Сд�����У�Ĭ�ϵõ����һ���Ӷ���)
	// �ɹ�,���ض���ָ��,����,����NULL
	virtual IResObject* GetSubObjectByName(LPCSTR szObjectName = NULL) = 0;
	// ͨ�������Ż�ȡ�Ӷ���
	// �ɹ�,���ض���ָ��,����,����NULL
	virtual IResObject* GetSubObjectByIndex(int nIndex) = 0;
	// ��ĳ��������Ϊ�Ӷ���(������Ķ������Ӷ�����ͨ����ϣ����ʣ�
	// ����SaveScript()�����¼��ز��ܷ���)����Ҫ���������ű��ĺϲ�
	virtual BOOL AddSubObject(IResObject* pResObject) = 0;
	// ɾ���Ӷ���(Ĭ��ɾ��β����)
	virtual BOOL EraseSubObjectByName(LPCSTR szObjectName = NULL) = 0;
	// ɾ��ָ�������Ķ���
	virtual BOOL EraseSubObjectByIndex(int nIndex) = 0;

	// �������
	// �ɹ�����TRUE,������������,��ʧ��,����FALSE
	virtual BOOL AddProperty(LPCSTR szPropertyName, LPCSTR szPropertyValue = NULL) = 0;
	// ɾ��ָ��������
	// �ɹ�����TRUE,�粻���ڴ�����,��ʧ��,����FALSE
	virtual BOOL EraseProperty(LPCSTR szPropertyName) = 0;
	// �޸���������
	// �ɹ�����TRUE,�粻���ڴ�����,��ʧ��,����FALSE
	virtual BOOL SetPropertyName(LPCSTR szOldPropertyName, LPCSTR szNewPropertyName) = 0;

	// ��ȡ����ֵ
	virtual LPCSTR GetPropertyString(LPCSTR szPropertyName) = 0;
	virtual BOOL GetPropertyLong  (LPCSTR szPropertyName, long& nReturn, long nDefault = 0L) = 0;
	virtual BOOL GetPropertyFloat (LPCSTR szPropertyName, double& dlReturn,	double dlDefault = 0.0) = 0;
	
	// ��������ֵ
	// ���ָ�������������������Զ�����һ������
	virtual BOOL SetPropertyString(LPCSTR szPropertyName, LPCSTR szSetString) = 0;
	virtual BOOL SetPropertyLong  (LPCSTR szPropertyName, long nSet) = 0;
	virtual BOOL SetPropertyFloat (LPCSTR szPropertyName, double dlSet) = 0;

	// ��ȡ��������鳤��(�����������鳤�Ȼ��ȡ�������ʵ�ʳ���)
	// ע1�����lpData=NULL,�������鳤��
	// ע2�����nCountָ��ĳ��ֵ,�򷵻�����ǰnCount��ֵ,Ĭ��(nCount=0)��������ֵ,
	//		���nCount��Խ�����ʵ�ʳ���,�򷵻�ʵ�ʳ���)
	virtual int GetArrayLong (LPCSTR szArrayName, long*   lpData = NULL, int nCount = 0) = 0;
	virtual int GetArrayFloat(LPCSTR szArrayName, double* lpData = NULL, int nCount = 0) = 0;
	virtual int GetArrayByte (LPCSTR szArrayName, BYTE*   lpData = NULL, int nCount = 0) = 0;
	
	// ��������(������������ʱ���޸�,�����½�����)
	// ����: ������,���黺������鳤��
	virtual BOOL SetArrayLong (LPCSTR szArrayName, long*   lpData, int nCount) = 0;
	virtual BOOL SetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount) = 0;
	virtual BOOL SetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount) = 0;

	// ���Ʊ����󣬲������Ӷ����б�Ĭ�ϲ���ϵͳ�����ID�ţ��������ϣ��
	virtual IResObject* Duplicate(BOOL bUseNewID = TRUE) = 0;
	// ���Ʊ������������Ӷ���Ĭ�ϲ���ϵͳ�����ID�ţ��������ϣ��
	// virtual IResObject* DuplicateAll(BOOL bUseNewID = TRUE) = 0;
	virtual IResScript* GetResScript() = 0;

#ifdef _DEBUG
	virtual BOOL Dump() = 0;
#endif
};
//------------------------------------------------------------------------
// ��Դ�ű��ӿ�
// ע�⣺		ÿ��.res��Դ�ļ���Ӧһ����Դ�ű��ӿ�
//				��IResScript����ʹ��ʱ,һ��ҪRelease()
struct IResScript
{
	// ���������б�
	virtual BOOL LoadTypeList(LPCSTR szTypeListFileName) = 0;

	// ������Դ�ű�
	virtual BOOL SaveScript(LPCSTR szResScriptFileName = NULL, BYTE* pKey = NULL) = 0;

	// ����ɶ������ļ�
	virtual BOOL Compile(LPCSTR szFileName = NULL) = 0;

	// ����ָ��ID����Դ����(ͨ����ϣ�����.res�ļ��������������ж���)
	virtual IResObject* GetResObjectRoot() = 0; // ID = 0
	virtual IResObject* GetResObject(UINT uID) = 0;

	// ��ñ��ű������ж�������
	virtual int GetObjectCount() = 0;

	// ��ñ��ű����ļ���
	virtual LPCSTR GetScriptFileName() = 0;
	// ���MaxID
	virtual UINT GetMaxID() = 0;

	// Ŀǰ�������ļ������������صȲ���
	// ���������.res�ļ���ָ��ID�Ķ�����ַ�������(�����ڷ�������)
	virtual BOOL ParseObjectServer(UINT uID, char* pData) = 0;
	// ��һ������Ľű��ַ���������һ���ڴ����(�����ڿͻ���)
	virtual IResObject* ParseObjectClient(char* pData, int nSize) = 0;

	virtual BOOL AddToHashMap(UINT uID, IResObject* pResObject) = 0;

	// �ͷ���Դ�ű�(�������ж�����б���ͷ�)
	virtual BOOL Release() = 0;

#ifdef _DEBUG
	virtual BOOL Dump() = 0;
#endif
};
//------------------------------------------------------------------------
// ��������Դ�ű��ӿ�
// ע�⣺		ÿ����������Դ�ļ���Ӧһ����������Դ�ű��ӿ�
//				��IBinScript����ʹ��ʱ,һ��ҪRelease()
struct IBinScript
{
	// ����ָ��ID�Ķ�������Դ����
	// ʧ�� ����NULL
	virtual HANDLE GetObject(UINT uID) = 0;

	// ��ñ��ű������ж�������
	virtual int GetObjectCount() = 0;

	// �ͷ�
	virtual BOOL Release() = 0;
  
    // ��ȡ����Ķ��������ݻ���
	// �ɹ� ���ػ���ָ�룻ʧ�� ����NULL
	virtual BYTE* GetBuffer(HANDLE hBinObject) = 0;

	// �����������Ļ����С
	// ʧ�� ����-1
	virtual int GetSize(HANDLE hBinObject) = 0;

	// ��ò��̶��������Ե���Ŀ
	// ʧ�� ����-1
	virtual int GetUnFixPropertyCount(HANDLE hBinObject) = 0;
	
	// ��ò��̶��������Ե�ƫ����
	// ʧ�� ����-1
	// ע: nIndexExָ���̶����Ե�������(0,1,...,n),���������й̶�
	// ���ȵ���Щ���ԣ���һ�����̶����Ե�����Ϊ0���ڶ���1...����
	virtual int GetUnFixPropertyOffset(HANDLE hBinObject, int nIndexEx) = 0;

	// ��ò��̶��������Ե�ʵ�ʳ���
	// ʧ�ܷ���-1
	// ע: nIndexExָ���̶����Ե�������(0,1,...,n),���������й̶�
	// ���ȵ���Щ���ԣ���һ�����̶����Ե�����Ϊ0���ڶ���1...����
	virtual int GetUnFixPropertyLength(HANDLE hBinObject, int nIndexEx) = 0;

	// ��ȡ�Ӷ�����Ŀ
	// ʧ�ܷ���-1
	virtual int GetSubObjectCount(HANDLE hBinObject) = 0;

	// ����������Ӷ���
	// �ɹ� �����Ӷ�������ʧ�� ����-1
	virtual HANDLE GetSubObject(HANDLE hBinObject, int nIndex) = 0;

	// ɾ��ָ������Ķ���(�������µ��Ӷ���)
	virtual BOOL Delete(UINT uID) = 0;
};
//------------------------------------------------------------------------
// ��Դ������
// Ҫ���ص���Դ����
#define RES_BMP				0x00000001L
#define RES_TGA				0x00000002L
#define RES_JPG				0x00000004L
#define RES_RLE				0x00000008L
#define RES_SOUND			0x00000010L
#define RES_MUSIC			0x00000020L
#define RES_DATA			0x00000040L

struct IResLoader
{
	virtual HANDLE	LoadRes(const char* szFileName, UINT uType) = 0;
	virtual HBITMAP LoadBMP(const char* szResName) = 0;
	virtual HBITMAP LoadTGA(const char* szResName) = 0;
	virtual HBITMAP LoadJPG(const char* szResName) = 0;
};
//------------------------------------------------------------------------
// ��Դ�������ӿ�
struct IResMgr
{
	// �½��յ���Դ�ű�(�������ϲ�����б�ָ��,nStartID��������Դ�ű�ID����ʼֵ)
	virtual IResScript* CreateResScript(int nStartID = 0) = 0;

	// ������������Դ�ļ�
	virtual IResScript* LoadResScript(LPCSTR szResFileName, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL) = 0;
	virtual IResScript* LoadResScript(IFileObject* pFileObject, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL) = 0;

	// ���ض����ƽű�
	virtual IBinScript* LoadBinScript(LPCSTR szBinFileName) = 0;
	virtual IBinScript* LoadBinScript(IFileObject* pFileObject) = 0;

	// ͨ���ļ��б�ϲ���Դ�ļ�
	virtual BOOL MergeResourceScriptByFileList(LPCSTR szListFileName, LPCSTR szMergeFileName) = 0;

	// �ͷ���Դ������(ֻ�ͷ��Լ�,���ͷŴ�����������IResScript*ָ���)
	virtual BOOL Release() = 0;
};

struct ITrace;
//------------------------------------------------------------------------
/*
	��Դ������������

	���÷�ʽ��
	1���������Ŀ�ڴ��� CIResMgrHelper ȫ�ֶ���
	2������ Create() ��ȡ IResMgr ����
	3������ IResMgr �� GetResScript() ��ȡĳ��Դ�ļ��� IResScript��
	4��IResScript ���� Create()��LoadScript()����ö����б�IResObjectList,Ҳ����SaveScript()��
	5��IResObjectList����AddResObject()��EraseResObject()�Ȼ�ö�Ӧ��IResObject��
	6���� IResObject ���ж������Ե���ɾ�ģ�������ɾ��һ���� IResObject �б�
	7��IResScript->Release()�ͷ����еĶ�����б�
	8��CIResMgrHelper ȫ�ֶ�������������Զ�����Close�ͷ�ȫ������

	ע������5��6����ѭ����������ʹ��
*/
class CIResMgrHelper : public TSingletonClass< CIResMgrHelper >
{
public:
	CIResMgrHelper()
	{
		m_pResMgr = NULL;
	}
	~CIResMgrHelper()
	{
		Close();	
	}
	BOOL CreateBinScript(DWORD dwExpectVersion, const char* szBinFileName,
		ITrace* pTrace, IBinScript** ppBinScript);
	BOOL CreateResScript(DWORD dwExpectVersion, const char* szResFileName, BOOL bIgnoreID,
		ITrace* pTrace, IResScript** ppResScript);
	BOOL CreateResMgr(DWORD dwExpectVersion,ITrace *pTrace,IResMgr **ppResMgr);
	IResMgr* Create(ITrace *pTrace)
	{
		Close();
		if(!CreateResMgr(VERSION_QUERY_RESOURCE_MANAGER,pTrace,&m_pResMgr))
		{
			return NULL;
		}
		return m_pResMgr;
	}
	void Close()
	{
		if (m_pResMgr)
		{
			m_pResMgr->Release();
			m_pResMgr = NULL;
		}
	}
public:
	IResMgr* m_pResMgr;
};
//------------------------------------------------------------------------

