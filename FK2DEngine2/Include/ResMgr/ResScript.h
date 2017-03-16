/**
*	created:		2012-6-18   20:16
*	filename: 		ResScript
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <hash_map>
using stdext::hash_map;
//------------------------------------------------------------------------
class CResScript  : public IResScript
{	
private:
	int							m_nCount;				// ���ж�������
	CResObject*					m_pResObjectRoot;		// ������
	IFileObject*				m_pFileObject;			// �ļ�����ָ��
	hash_map<UINT, CResObject*>	m_HashMap;				// �����IDӳ���
	hash_map<string, TYPEINFO*>	m_TypeList;				// ����ģ��ӳ���
	bool						m_bReleaseFileObject;	// �ļ������Ƿ���Ҫ�ͷŵı�־
public:
	CResScript();
	virtual ~CResScript();

	// IResScript
	// �½��յ���Դ�ű�(�������ϲ�����б�ָ��)
	BOOL Create(int nStartID = 0);
	// ������Դ�ű�(�������ϲ�����б�ָ��)
	BOOL LoadScript(IFileObject* pFileObject, BOOL bIgnoreID, bool bReleaseFileObject, BYTE* pKey = NULL);
	BOOL LoadTypeList(LPCSTR szTypeListFileName);
	// ������Դ�ű�
	BOOL SaveScript(LPCSTR szResScriptFileName = NULL, BYTE* pKey = NULL);
	// ����
	BOOL Compile(LPCSTR szFileName = NULL);
	// ����ָ��ID����Դ����(�ڲ�ͨ����ϣ����ٷ���)
	IResObject* GetResObject(UINT uID);
	IResObject* GetResObjectRoot();
	// ��ñ��ű������ж�������
	int GetObjectCount();
	// ��ñ��ű����ļ���
	LPCSTR GetScriptFileName();
	UINT GetMaxID();
	// Ŀǰ�������ļ������������صȲ���
	BOOL ParseObjectServer(UINT uID, char* pData);
	IResObject* ParseObjectClient(char* pData, int nSize);
	// �ͷ���Դ�ű�
	BOOL Release();

	BOOL AddToHashMap(UINT uID, IResObject* pResObject);
	BOOL EraseFromHashMap(UINT uID);
	BOOL WriteResObject(IResObject* pResObject, FILE* fp);
	LPCSTR GetPath();
	UINT NextMaxID();
	void Empty();
	BOOL WriteBinObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp);
	BOOL WriteRootObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp);
#ifdef _DEBUG
	BOOL Dump();
#endif
};
//------------------------------------------------------------------------