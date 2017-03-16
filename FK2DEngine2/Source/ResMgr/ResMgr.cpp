/**
*	created:		2012-6-18   22:18
*	filename: 		ResMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/ResMgr/ResMgrCommon.h"
#include "../../Include/ResMgr/ResObject.h"
#include "../../Include/ResMgr/ResScript.h"
#include "../../Include/ResMgr/BinScript.h"
#include "../../Include/ResMgr/ResMgr.h"
//------------------------------------------------------------------------
CResMgr::CResMgr()
{
}
//------------------------------------------------------------------------
CResMgr::~CResMgr()
{
}
//------------------------------------------------------------------------
// ����IResScript(��Դ�ű��ӿ�)
IResScript* CResMgr::CreateResScript(int nStartID/* = 0*/)
{
	CResScript* pResScript = new CResScript();
	Assert(pResScript);
	if (!pResScript->Create(nStartID))
	{
		SAFE_RELEASE(pResScript);
		return NULL;
	}

	TraceLn("������Դ�ű� [nStartID=%d] ���", nStartID);
	return static_cast<IResScript*>(pResScript);
}
//------------------------------------------------------------------------
// ������Դ�ű�
IResScript* CResMgr::LoadResScript(LPCSTR szResFileName, BOOL bIgnoreID, BYTE* pKey)
{
	IFileObject* pFileObject = _create_fileobject(szResFileName);
	Assert(pFileObject);
	CResScript* pResScript = new CResScript();
	Assert(pResScript);
	if (!pResScript->LoadScript(pFileObject, bIgnoreID, true, pKey))
	{
		SAFE_RELEASE(pResScript);
		return NULL;
	}

	TraceLn("������Դ�ű� ['%s'] ���", pFileObject->GetFilePath());
	return static_cast<IResScript*>(pResScript);
}
//------------------------------------------------------------------------
// ������Դ�ű�
IResScript* CResMgr::LoadResScript(IFileObject* pFileObject, BOOL bIgnoreID, BYTE* pKey)
{
	CResScript* pResScript = new CResScript();
	Assert(pResScript);
	if (!pResScript->LoadScript(pFileObject, bIgnoreID, false, pKey))
	{
		SAFE_RELEASE(pResScript);
		return NULL;
	}

	TraceLn("������Դ�ű� ['%s'] ���", pFileObject->GetFilePath());
	return static_cast<IResScript*>(pResScript);
}
//------------------------------------------------------------------------
// ���ض����ƽű��ļ�
IBinScript* CResMgr::LoadBinScript(LPCSTR szBinFileName)
{
	IFileObject* pFileObject = _create_fileobject(szBinFileName);
	Assert(pFileObject);
	CBinScript* pBinScript = new CBinScript();
	Assert(pBinScript);
	if (!pBinScript->LoadBinScript(pFileObject, true))
	{
		SAFE_RELEASE(pBinScript);
		return NULL;
	}

	TraceLn("������Դ�ű� ['%s'] ���", pFileObject->GetFilePath());
	return static_cast<IBinScript*>(pBinScript);
}
//------------------------------------------------------------------------
IBinScript* CResMgr::LoadBinScript(IFileObject* pFileObject)
{
	CBinScript* pBinScript = new CBinScript();
	Assert(pBinScript);
	if (!pBinScript->LoadBinScript(pFileObject, false))
	{
		SAFE_RELEASE(pBinScript);
		return NULL;
	}

	TraceLn("������Դ�ű� ['%s'] ���", pFileObject->GetFilePath());
	return static_cast<IBinScript*>(pBinScript);
}
//------------------------------------------------------------------------
// �ͷ���Դ������(������������������IResScript*, IBinScript*, IResLoader*)
BOOL CResMgr::Release()
{
	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResMgr::MergeResourceScriptByFileList(LPCSTR szListFileName, LPCSTR szMergeFileName)
{
	// �����ļ��б�
	IResScript* pScript_FileList = LoadResScript(szListFileName, TRUE);
	Assert(pScript_FileList);
	IResObject* pRoot_FileList = pScript_FileList->GetResObjectRoot();
	DWORD dwFileCount = pRoot_FileList->GetSubObjectCount();

	// ����ÿ���ű��ļ���IResScript*�ӿ�����
	IResScript** ppScript = new IResScript*[dwFileCount];
	Assert(ppScript);

	// ��������Դ�ļ��ϲ�����һ����Դ�ļ�
	for (int i=0; i<dwFileCount; i++)
	{
		IResObject* pObject = NULL;									// �ļ��б��еĵ�n���ļ�����
		long lIgnoreID = 0;
		pObject = pRoot_FileList->GetSubObjectByIndex(i);
		pObject->GetPropertyLong("ignoreID", lIgnoreID);
		string strFileName = pObject->GetPropertyString("src");

		ppScript[i] = LoadResScript(strFileName.c_str(), lIgnoreID); // ������n����Դ�ű��ļ�
		IResObject* pRoot = ppScript[i]->GetResObjectRoot();
		DWORD dwObjectCount = pRoot->GetSubObjectCount();
		if (i != 0)
		{
			for (int k=0; k<dwObjectCount; k++)						// ��n����Դ�ű��ļ���Root�����е��Ӷ���һ�������
			{
				IResObject* obj = pRoot->GetSubObjectByIndex(k);
				ppScript[0]->GetResObjectRoot()->AddSubObject(obj);
				static_cast<CResScript*>(ppScript[0])->AddToHashMap(obj->GetID(), obj);
			}
		}
	}

	// ����ϲ������Դ�ű�
	ppScript[0]->SaveScript(szMergeFileName);

	// �ͷ�1��n���ű��ļ�����Դ
	for (int j=0; j<dwFileCount; j++)
		SAFE_RELEASE(ppScript[j]);
	delete[] ppScript;

	// �ͷŲ����ļ��б��IResScript�ӿ�
	SAFE_RELEASE(pScript_FileList);

	return TRUE;
}
//------------------------------------------------------------------------