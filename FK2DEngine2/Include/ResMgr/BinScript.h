/**
*	created:		2012-6-18   21:34
*	filename: 		BinScript
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
class CBinScript  : public IBinScript
{	
private:
	hash_map<UINT, DWORD>		m_OffsetMap;			// ID-Offsetӳ���
	IFileObject*				m_pFileObject;			// �ļ�����
	DWORD						m_dwObjectCount;		// ��������
	hash_map<UINT, CBinObject*>	m_HashMap;				// ��¼�Ѿ������˵�BinObjectInfo
	bool						m_bReleaseFileObject;	// �ļ������Ƿ���Ҫ�ͷŵı�־
public:
	CBinScript();
	~CBinScript();
public:
	HANDLE GetObject(UINT uID);
	int GetObjectCount() {return m_dwObjectCount;}
	BOOL Release();

	BYTE* GetBuffer(HANDLE hBinObject);
	int GetSize(HANDLE hBinObject);
	int GetUnFixPropertyCount(HANDLE hBinObject);
	int GetUnFixPropertyOffset(HANDLE hBinObject, int nIndexEx);
	int GetUnFixPropertyLength(HANDLE hBinObject, int nIndexEx);
	int GetSubObjectCount(HANDLE hBinObject);
	HANDLE GetSubObject(HANDLE hBinObject, int nIndex);
	BOOL Delete(UINT uID);

	LPCSTR GetScriptFileName(){return m_pFileObject->GetFilePath();}
	void Empty();
	BOOL LoadBinScript(IFileObject* pFileObject, bool bReleaseFileObject);
	BOOL AddToHashMap(UINT uID, CBinObject* pBinObject);
	BOOL EraseFromHashMap(UINT uID);
};
//------------------------------------------------------------------------