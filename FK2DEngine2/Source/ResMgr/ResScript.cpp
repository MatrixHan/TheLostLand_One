/**
*	created:		2012-6-18   21:12
*	filename: 		ResScript
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../Include/ResMgr/ResMgrCommon.h"
#include "../../Include/ResMgr/ResDecoder.h"
#include "../../Include/ResMgr/ResObject.h"
#include "../../Include/ResMgr/BitTool.h"
#include "../../Include/ResMgr/ResScript.h"
#include "../../Include/FileSystem/StdFileObject.h"
#include "../../Include/ResMgr/Encrypt.h"
//------------------------------------------------------------------------
CResScript::CResScript()
{
	m_pResObjectRoot = NULL;
	m_pFileObject = NULL;
	m_bReleaseFileObject = false;

	Empty();
}
//------------------------------------------------------------------------
CResScript::~CResScript()
{
}
//------------------------------------------------------------------------
// �½��յ���Դ�ű�
BOOL CResScript::Create(int nStartID/* = 0*/)
{
	Empty();

	m_pResObjectRoot = new CResObject(this);
	Assert(m_pResObjectRoot);
	m_pResObjectRoot->SetObjectInfo("Root", NULL);
	m_pResObjectRoot->SetID(0);
	m_HashMap[0] = m_pResObjectRoot;
	m_nCount ++;
	return m_pResObjectRoot->SetPropertyLong("MaxID", nStartID);
}
//------------------------------------------------------------------------
// ������Դ�ű�
BOOL CResScript::LoadScript(IFileObject* pFileObject, BOOL bIgnoreID, 
							bool bReleaseFileObject, BYTE* pKey)
{
	Assert(pFileObject);

	m_pFileObject = pFileObject;
	m_bReleaseFileObject = bReleaseFileObject;

	// �������ָ��
	m_pResObjectRoot = new CResObject(this);
	Assert(m_pResObjectRoot);
	m_pResObjectRoot->SetObjectInfo("Root", NULL);
	m_pResObjectRoot->SetID(0);
	m_pResObjectRoot->SetPropertyLong("MaxID", 0);
	m_HashMap[0] = m_pResObjectRoot;
	m_nCount ++;

	CResDecoder ResDecoder(this, m_pResObjectRoot);
	// �����ű��ļ�
	if (!ResDecoder.ParseFile(pFileObject, bIgnoreID, pKey))
	{
		SAFE_RELEASE(m_pResObjectRoot);
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// ���������б�
BOOL CResScript::LoadTypeList(LPCSTR szTypeListFileName)
{
	Assert(szTypeListFileName != NULL && *szTypeListFileName != 0);

	// �������Ͷ���
	hash_map<string, DATATYPE>	mapType;
	mapType["bool"]		= dt_bool;
	mapType["char"]		= dt_char;
	mapType["short"]	= dt_short;
	mapType["int"]		= dt_int;
	mapType["float"]	= dt_float;
	mapType["byte"]		= dt_byte;
	mapType["word"]		= dt_word;
	mapType["dword"]	= dt_dword;
	mapType["string"]	= dt_string;

	FILE* pFile = NULL;
	pFile = fopen(szTypeListFileName, "r");
	if (pFile == NULL)
		return FALSE;

	string strLine(MAX_PATH, 0);
	char buf[MAX_PATH];
	TYPEINFO* pTypeInfo = NULL;

	while (fgets(buf, MAX_PATH, pFile))
	{
		strLine = buf;
		if (_find_string_no_case(strLine, "struct")) // ����
		{
			// ȥ��"struct"
			_trim(strLine);
			_erase_left(strLine, 6);

			string strTypeName = strLine;
			string strTypeID = strLine;
			_left_c(strTypeName, '=');
			_right_c(strTypeID, '=');

			pTypeInfo = new TYPEINFO;
			Assert(pTypeInfo);
			pTypeInfo->name = _trim(strTypeName);
			pTypeInfo->cbID = atoi(_trim(strTypeID));
			Assert(pTypeInfo->cbID != 0); // 0ΪϵͳĬ�ϵ���ЧID

			// ����ӳ���
			m_TypeList[_make_lower(strTypeName)] = pTypeInfo;
		}
		else if (_find_string_no_case(strLine, "dim")) // ����
		{
			PROPINFO pi;
			string str = strLine;

			// ȥ��"dim"
			_trim(strLine);
			_erase_left(strLine, 3);
			_trim(strLine);

			int pos;
			if ((pos = _find_char(strLine, ' ')) || 
				(pos = _find_char(strLine, '\t'))) // ��������˵��
			{
				string strType = strLine;
				_left(strType, pos - 1);
				_make_lower(strType);
				hash_map<string, DATATYPE>::iterator it = mapType.find(strType.c_str());
				if (it != mapType.end())
				{
					pi.type = (*it).second;
				}
				else
				{
					FKResTraceColorLn(MY_TRACE_COLOR, L"���� CResScript::LoadTypeList() �����쳣���޷�ʶ�����������:%s !", strType.c_str());
					return FALSE;
				}

				// ȥ����������˵����(int dword,word,long,char,...)
				_erase_left(strLine, pos - 1);
				_trim(strLine);

				str = strLine;
				if (pos = _find_char(strLine, ':')) // ָ������
				{
					// ���������
					_left(str, pos - 1);
					_trim(str);
					pi.propname = str;

					// �����
					_erase_left(strLine, pos);
					_trim(strLine);
					pi.length = atoi(strLine.c_str());
					switch (pi.type)
					{
					case dt_undefine:
						FKResTraceColorLn(MY_TRACE_COLOR, 
							L"���� CResScript::LoadTypeList() �����쳣����Ч���������� dt_undefine ��");
						return FALSE;

					case dt_bool:
					case dt_char:
					case dt_byte:
						{
							if (pi.length > 8)
							{
								FKResTraceColorLn(MY_TRACE_COLOR,
									L"���� CResScript::LoadTypeList() ���ִ����ֽڹ�����");
								return FALSE;
							}
						}
						break;
					case dt_short:
					case dt_word:
						{
							if (pi.length > 16)
							{
								FKResTraceColorLn(MY_TRACE_COLOR,
									L"���� CResScript::LoadTypeList() ���ִ����ֽڹ�����");
								return FALSE;
							}
						}
						break;
					case dt_int:
					case dt_dword:
					case dt_float:
						{
							if (pi.length > 32)
							{
								FKResTraceColorLn(MY_TRACE_COLOR,
									L"���� CResScript::LoadTypeList() ���ִ����ֽڹ�����");
								return FALSE;
							}
						}
						break;
					case dt_string:
						break;
					}

					pi.isArray = false;
				}
				else // ûָ����
				{
					if (pos = _find_char(strLine, '[')) // ָ������
					{
						// ���������
						_left(str, pos - 1);
						_trim(str);
						pi.propname = str;

						// ������鳤��
						_erase_right(strLine, 1);
						_erase_left(strLine, pos);
						_trim(strLine);

						pi.length = atoi(strLine.c_str());
						pi.isArray = true;
					}
					else // ûָ������
					{
						pi.propname = str;
						pi.isArray = false;
						switch (pi.type)
						{
						case dt_undefine:
							FKResTraceColorLn(MY_TRACE_COLOR,
								L"���� CResScript::LoadTypeList() �����쳣����Ч���������� dt_undefine ��");
							return FALSE;

						case dt_bool:
						case dt_char:
						case dt_byte:
							pi.length = 8;
							break;
						case dt_short:
						case dt_word:
							pi.length = 16;
							break;
						case dt_int:
						case dt_dword:
						case dt_float:
							pi.length = 32;
							break;
						case dt_string:
							pi.length = 0;
							pi.isArray = true;
							break;
						}
					}
				}
				// ��¼
				if (pTypeInfo)
				{
					pTypeInfo->prop.push_back(pi);
					int size=pTypeInfo->prop.size();
				}
			}
			else
				TraceLn("��Ч����������: %s", str.c_str());
		}
	}

	fclose(pFile);
	return TRUE;
}
//------------------------------------------------------------------------
// ������Դ�ű�(�Ժ����Ҫ��������IFileObject�Ľӿ�)
BOOL CResScript::SaveScript(LPCSTR szResScriptFileName, BYTE* pKey)
{
	Assert(m_pResObjectRoot);

	FILE* fp = NULL;
	if (szResScriptFileName == NULL) // Ĭ��
	{
		VerifyR(!m_pFileObject, FALSE, 
			"CResScript::SaveScript() �����쳣���ļ���Ϊ�գ�");
		fp = fopen(m_pFileObject->GetFilePath(), "w");
	}
	else
	{
		if (m_pFileObject == NULL)
		{
			m_pFileObject = _create_fileobject(szResScriptFileName);
			Assert(m_pFileObject);
		}
		fp = fopen(szResScriptFileName, "w");
	}

	Assert(fp);

	// д��ͷ
	char header[]="FreeKnight��Դ�ű�\n";  /*RESSCRIPT\n*/  /*"///////////////////////////////\n// *******  ��Դ�ű�  *******\n//\n//  ÿ����󳤶�2048�ֽ�\n///////////////////////////////\n\n";*/
	fwrite(header, lstrlenA(header), 1, fp);

	// д���ڶ��������
	int nCount = m_pResObjectRoot->GetPropertyCount();
	char s[256];
	for (int i = 0; i < nCount; i++)
	{
		sprintf(s, "%s = %s\n", m_pResObjectRoot->GetPropertyName(i), m_pResObjectRoot->GetPropertyValue(i));
		fwrite(s, lstrlenA(s), 1, fp); // ... = ...
	}

	// д��������
	WriteResObject(m_pResObjectRoot, fp);

	// �ر��ļ�
	fclose(fp);

	if (pKey)
	{
		CStdFileObject file;
		file.SetFilePath(szResScriptFileName);
		// ���ļ�
		if (!file.Open("rb"))
		{
			FKResTraceColorLn(MY_TRACE_COLOR, 
				L"���� CResScript::SaveScript() �����쳣���޷����ļ� %s", file.GetFilePath());
			return FALSE;
		}

		// ����ļ���С
		int uFileLen = file.GetLength();

		// �����ڴ�
		BYTE* pBuffer = new BYTE[uFileLen + 3];
		Assert(pBuffer);

		// ����
		file.SeekToBegin();
		file.Read(pBuffer, uFileLen);
		file.Close();

		CEncrypt en;
		en.Encrypt(pBuffer, uFileLen + 3, pKey);

		// ���ļ�
		if (!file.Open("wb"))
		{
			FKResTraceColorLn(MY_TRACE_COLOR, 
				L"���� CResScript::SaveScript() �����쳣���޷����ļ� %s��", file.GetFilePath());
			return FALSE;
		}
		file.Write(pBuffer, uFileLen);
		file.Close();
	}

	return TRUE;
}
//------------------------------------------------------------------------
// ����
BOOL CResScript::Compile(LPCSTR szFileName/* = NULL*/)
{
	if (!m_pResObjectRoot)
		return	FALSE;

	VerifyR(m_TypeList.empty(), FALSE,
		"CResScript::Compile() �����쳣���޷���ȡ TypeList ���ļ���");

	FILE* fp = NULL;
	if (szFileName == NULL) // Ĭ��
	{
		VerifyR(!m_pFileObject, FALSE, 
			"CResScript::Compile() �����쳣���ļ���Ϊ�գ�");

		string strFileName = m_pFileObject->GetFilePath();
		int pos = strFileName.find_last_of('.') + 1;
		if (pos)
			_left(strFileName, pos - 1);

		strFileName += ".rcf";
		fp = fopen(strFileName.c_str(), "wb");
	}
	else
	{
		fp = fopen(szFileName, "wb");
	}
	Assert(fp);

	int nFilePos = 0;
	vector<ID_OFFSET>	idOffsetTable; // ���ڼ�¼ID-OFFSETӳ��

	// д�ļ�ͷ
	BINOBJECTHEADER bohd;
	fwrite(&bohd, 1, sizeof(BINOBJECTHEADER), fp);
	nFilePos += sizeof(BINOBJECTHEADER);

	// д����
	WriteRootObject(m_pResObjectRoot, idOffsetTable, nFilePos, fp);
	WriteBinObject(m_pResObjectRoot, idOffsetTable, nFilePos, fp);

	// дӳ���
	fwrite(&idOffsetTable[0], 1, idOffsetTable.size()*sizeof(ID_OFFSET), fp);

	// ��д�ļ�ͷ
	fseek(fp, 0, SEEK_SET);
	bohd.dwMapOffset = nFilePos;
	bohd.dwObjectCount = idOffsetTable.size();
	fwrite(&bohd, 1, sizeof(BINOBJECTHEADER), fp);

	// �ر��ļ�
	fclose(fp);
	return TRUE;
}
//------------------------------------------------------------------------
IResObject* CResScript::GetResObjectRoot()
{
	return GetResObject(0);
}
//------------------------------------------------------------------------
// ����ָ��ID����Դ����(�ڲ�ͨ����ϣ����ٷ���)
IResObject* CResScript::GetResObject(UINT uID)
{
	if (uID == 0)
		return static_cast<IResObject*>(m_pResObjectRoot);

	CResObject* pResObject = NULL;
	hash_map<UINT, CResObject*>::iterator it = m_HashMap.find(uID);
	if (it != m_HashMap.end())
	{
		pResObject = (*it).second;
		Assert(pResObject != NULL);
		return pResObject;
	}
	return NULL;
}
//------------------------------------------------------------------------
// ��ñ��ű������ж�������
int CResScript::GetObjectCount()
{
	return m_nCount;
}
//------------------------------------------------------------------------
// ��ñ��ű����ļ���
LPCSTR CResScript::GetScriptFileName()
{
	if (m_pFileObject == NULL)
		return NULL;
	return m_pFileObject->GetFilePath();
}
//------------------------------------------------------------------------
// �ͷ���Դ�ű�
BOOL CResScript::Release()
{
#ifdef _DEBUG
	char szbuf[256];
	lstrcpyA(szbuf, "�ͷ���Դ�ű� ['���ļ�'] ��ɡ�");
	if (m_pFileObject)
		sprintf(szbuf, "�ͷ���Դ�ű� ['%s'] ���\r\n", GetScriptFileName());
#endif

	Empty();

	if (!m_TypeList.empty())
	{
		hash_map<string, TYPEINFO*>::iterator it;
		for (it = m_TypeList.begin(); it != m_TypeList.end(); it ++)
			((*it).second)->Release();
		m_TypeList.clear();
	}

#ifdef _DEBUG
	OutputDebugStringA(szbuf);
#endif

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResScript::AddToHashMap(UINT uID, IResObject* pResObject)
{
	Assert(pResObject != NULL);
	CResObject* pCResObject = static_cast<CResObject*>(pResObject);
	if (uID == 0)
		uID = NextMaxID();
#ifdef _DEBUG
	if (GetResObject(uID))
	{
		FKResTraceColorLn(MY_TRACE_COLOR, L"ID��ͻ��ID = %d��", uID);
	}
#endif
	m_HashMap[uID] = pCResObject;
	pCResObject->SetID(uID);
	if (pCResObject->m_pResScript == NULL)
		pCResObject->m_pResScript = this;
	m_nCount ++;
	return pResObject != NULL;
}
//------------------------------------------------------------------------
BOOL CResScript::EraseFromHashMap(UINT uID)
{
	CResObject* pResObject = NULL;
	hash_map<UINT, CResObject*>::iterator it = m_HashMap.find(uID);
	if (it != m_HashMap.end())
	{
		pResObject = (*it).second;
		Assert(pResObject != NULL);
		m_HashMap.erase(uID);
		m_nCount --;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// д����Ľű�(�ݹ麯��)
BOOL CResScript::WriteResObject(IResObject* pResObject, FILE* fp)
{
	Assert(pResObject != NULL && fp != NULL);

	// �����Tab�ַ�
	static char str[2148];
	static int nTabCount = -1;
	nTabCount ++;
	memset(str, '\t', nTabCount);

	int nObjNum = pResObject->GetSubObjectCount();

	for (int num = 0; num < nObjNum; num ++)
	{
		IResObject* pSubObject = pResObject->GetSubObjectByIndex(num);
		if (!pSubObject)
		{
			FKResTraceColorLn(MY_TRACE_COLOR, 
				L"���� CResScript::WriteResObject() �����쳣������ %s �ı��Ϊ NO.%d ���Ӷ��󲻴��ڣ�", 
				pResObject->GetObjectName(), num + 1, pResObject->GetID());
			return FALSE;
		}

		// д��������
		if (lstrlenA(pSubObject->GetTypeName()) > 0)
			sprintf(str + nTabCount, "Object %s:%s = %d\n", pSubObject->GetObjectName(), pSubObject->GetTypeName(), pSubObject->GetID());
		else
			sprintf(str + nTabCount, "Object %s = %d\n", pSubObject->GetObjectName(), pSubObject->GetID());
		fprintf(fp,"\n");
		fwrite(str, lstrlenA(str), 1, fp); // Object ...

		sprintf(str + nTabCount, "{\n");
		fwrite(str, lstrlenA(str), 1, fp); // д"{"

		// д����
		int nCount = pSubObject->GetPropertyCount();
		for (int i = 0; i < nCount; i++)
		{
			sprintf(str + nTabCount, "%c%s = %s\n", '\t', pSubObject->GetPropertyName(i), pSubObject->GetPropertyValue(i));
			fwrite(str, lstrlenA(str), 1, fp); // ... = ...
		}


		if (pSubObject->GetSubObjectCount() > 0) // д�Ӷ���
		{
			WriteResObject(pSubObject, fp);
		}

		sprintf(str + nTabCount, "}\n");
		fwrite(str, lstrlenA(str), 1, fp); // д"}"
	}
	nTabCount --;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResScript::WriteBinObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp)
{
	Assert(pResObject != NULL && fp != NULL);
	//dt_undefine,dt_bool,dt_char,dt_short,dt_int,dt_float,dt_byte,dt_word,dt_dword,dt_string
	static int TypeBytes[] = {0,1,1,2,4,4,1,2,4,0};

	int nObjNum = pResObject->GetSubObjectCount();

	for (int num = 0; num < nObjNum; num ++)
	{
		IResObject* pSubObject = pResObject->GetSubObjectByIndex(num);

		BINOBJECTINFO oi;
		TYPEINFO* pTypeInfo;
		PROPINFO pi;
		CBitTool bt;

		// ��ñ����������
		string strType = pSubObject->GetTypeName();
		hash_map<string, TYPEINFO*>::iterator it = m_TypeList.find(_make_lower(strType));
		if (it != m_TypeList.end())
		{
			pTypeInfo = (*it).second;
		}
		else // �����������ģ�岻���ڻ�û�ж��壬�Թ�!
		{
			FKResTraceColorLn(MY_TRACE_COLOR, L"Ignore object[%d]: %s[%s]", pSubObject->GetID(), pSubObject->GetObjectName(), pSubObject->GetTypeName());
			if (pSubObject->GetSubObjectCount() > 0) // д�Ӷ���
				WriteBinObject(pSubObject, idOffsetTable, nFilePos, fp);
			continue;
		}

		// ���ö����������Ϊ��ʶ
		oi.cbIdentify = pTypeInfo->cbID;

		// �ṹ����
		// oi.wSize = sizeof(BINOBJECTINFO);

		// ����������ƫ��
		oi.wOffset = pTypeInfo->GetFixPropertyLength() + pTypeInfo->GetFixArrayLength();

		// ����Ӷ�����Ŀ���Ӷ����б��ID
		vector<DWORD> vSubObjectIDList;
		if (pSubObject->GetSubObjectCount() > 0)
		{
			oi.wSubObjectCount = pSubObject->GetSubObjectCount();
			//oi.wSize += oi.wSubObjectCount*sizeof(DWORD);
			for (int i = 0; i < oi.wSubObjectCount; i++)
				vSubObjectIDList.push_back(pSubObject->GetSubObjectByIndex(i)->GetID());
		}
		else
			oi.wSubObjectCount = 0;


		// ��ò�������������Ŀ�������������Եĳ���
		vector<PROPINFO> vProp = pTypeInfo->prop;
		vector<WORD>	vUnFixList;
		for (unsigned int j=0;j<vProp.size();j++)
		{
			pi = vProp[j];
			if (pi.IsUnFixProperty())
			{
				oi.cbUnFixCount ++;
				WORD nLen = 0;
				if (pi.type == dt_string) // ����һ��'\0'
				{
					LPCSTR scr = pSubObject->GetPropertyString(pi.propname.c_str());
					if (scr != NULL)
						nLen =1 + (WORD)(lstrlenA(scr));
					else // ���Բ����ڣ����ó���Ϊ0
						nLen = (WORD)0;
				}
				else
				{
					nLen =(WORD)pSubObject->GetArrayLong(pi.propname.c_str(), NULL);
					nLen *= TypeBytes[pi.type];
				}
				vUnFixList.push_back(nLen);
			}
		}
		//oi.wSize += oi.cbUnFixCount*sizeof(WORD);

		for(unsigned int k = 0; k < vProp.size(); k++) // ÿ������
		{
			PROPINFO pi = vProp[k];
			if (pi.IsFixProperty()) // �Ƕ�������
			{
				switch(pi.type)
				{
				case dt_undefine:
					FKResTraceColorLn(MY_TRACE_COLOR,L"CResScript::WriteBinObject() catch error: undefine type name[\"dt_undefine\"]!");
					return FALSE;
				case dt_bool:
				case dt_char:
				case dt_short:
				case dt_int:
				case dt_byte:
				case dt_word:
				case dt_dword:
					{
						long lg;
						if (!pSubObject->GetPropertyLong(pi.propname.c_str(), lg))
							lg = 0;
						bt.Push(pi.type, &lg, pi.length);
					}
					break;
				case dt_float:
					{
						double fl;
						if (!pSubObject->GetPropertyFloat(pi.propname.c_str(), fl))
							fl = 0.0f;
						float f = (float)fl;
						bt.PushFloat(&f);
					}
					break;
				}
			}
			else if (pi.IsFixArray()) // ��������
			{
				switch(pi.type)
				{
				case dt_undefine:
					FKResTraceColorLn(MY_TRACE_COLOR,L"CResScript::WriteBinObject() catch error: undefine type name[\"dt_undefine\"]!");
					return FALSE;
				case dt_bool:
				case dt_char:
				case dt_byte:
					{
						int array_len = pSubObject->GetArrayByte(pi.propname.c_str(), NULL);
						BYTE* data = NULL;
						if (array_len == 0) // �����ڸ�����,����
						{
							array_len = pi.GetLength();
							data = new BYTE[array_len];
							Assert(data);
							memset(data, 0, array_len);
						}
						else
						{
							data = new BYTE[array_len];
							Assert(data);
							memset(data, 0, array_len);
							pSubObject->GetArrayByte(pi.propname.c_str(), data);
						}
						bt.PushArray(pi.type, (void*)data, array_len);
						delete[] data;
					}
					break;
				case dt_short:
				case dt_word:
				case dt_int:
				case dt_dword:
					{
						int array_len = pSubObject->GetArrayLong(pi.propname.c_str(), NULL);
						long* data = NULL;
						if (array_len == 0) // �����ڸ�����,����
						{
							array_len = pi.GetLength();
							data = new long[array_len];
							Assert(data);
							memset(data, 0, array_len*sizeof(long));
						}
						else
						{
							data = new long[array_len];
							Assert(data);
							pSubObject->GetArrayLong(pi.propname.c_str(), data);
						}
						bt.PushArray(pi.type, (void*)data, array_len);
						delete[] data;
					}
					break;
				case dt_float:
					{
						int array_len = pSubObject->GetArrayFloat(pi.propname.c_str(), NULL);
						double* data = NULL;
						if (array_len == 0) // �����ڸ�����,����
						{
							array_len = pi.GetLength();
							data = new double[array_len];
							Assert(data);
							memset(data, 0, sizeof(data));
						}
						else
						{
							data = new double[array_len];
							Assert(data);
							pSubObject->GetArrayFloat(pi.propname.c_str(), data);
						}
						bt.PushArray(pi.type, (void*)data, array_len);
						delete[] data;
					}
					break;
				}
			}
			else if (pi.IsUnFixProperty()) // ����������
			{
				if (pi.type == dt_string)
				{
					LPCSTR src = pSubObject->GetPropertyString(pi.propname.c_str());
					if (src != NULL)
						bt.PushString(src);
				}
				else
				{
					int nSize = 0;
					if (pi.type == dt_float) // ������
					{
						nSize =pSubObject->GetArrayFloat(pi.propname.c_str(), NULL);
						if (nSize != 0)
						{
							double* pUnFixData = new double[nSize];
							Assert(pUnFixData);
							pSubObject->GetArrayFloat(pi.propname.c_str(), pUnFixData);
							bt.PushArray(pi.type, (void*)pUnFixData, nSize);
							delete[] pUnFixData;
						}
					}
					else
					{
						nSize =pSubObject->GetArrayLong(pi.propname.c_str(), NULL);
						if (nSize != 0)
						{
							long* pUnFixData = new long[nSize];
							Assert(pUnFixData);
							pSubObject->GetArrayLong(pi.propname.c_str(), pUnFixData);
							bt.PushArray(pi.type, (void*)pUnFixData, nSize);
							delete[] pUnFixData;
						}
					}
				}
			}
			else
				FKResTraceColorLn(MY_TRACE_COLOR, 
				L"���� CResScript::WriteBinObject() �����쳣���޷�ʶ������ԣ�");
		}

		oi.wSize = bt.GetDataLength();

		// д�ṹ
		fwrite(&oi, 1, sizeof(BINOBJECTINFO), fp);
		// д�Ӷ����б�
		if (!vSubObjectIDList.empty())
			fwrite(&vSubObjectIDList[0], 1, vSubObjectIDList.size()*sizeof(DWORD),fp);
		// д���������ݳ����б�
		if (!vUnFixList.empty())
			fwrite(&vUnFixList[0], 1, vUnFixList.size()*sizeof(WORD),fp);
		// д�û�����
		if (bt.GetDataLength())
		{
			// _data_xor((BYTE*)bt.GetDataBuffer(), bt.GetDataLength());
			fwrite(bt.GetDataBuffer(), 1, bt.GetDataLength(), fp);
		}

		ID_OFFSET idmap;
		idmap.id = pSubObject->GetID();
		idmap.offset = nFilePos;
		idOffsetTable.push_back(idmap);
		nFilePos += sizeof(BINOBJECTINFO) + 
			oi.wSubObjectCount*sizeof(DWORD) + oi.cbUnFixCount*sizeof(WORD) + oi.wSize;

		if (pSubObject->GetSubObjectCount() > 0) // д�Ӷ���
			WriteBinObject(pSubObject, idOffsetTable, nFilePos, fp);
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResScript::WriteRootObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp)
{
	BINOBJECTINFO oi;
	CBitTool bt;
	oi.cbIdentify = 0;
	oi.cbUnFixCount = 0;

	// ����Ӷ�����Ŀ���Ӷ����б��ID
	vector<DWORD> vSubObjectIDList;
	if (pResObject->GetSubObjectCount() > 0)
	{
		oi.wSubObjectCount = pResObject->GetSubObjectCount();
		for (int i = 0; i < oi.wSubObjectCount; i++)
			vSubObjectIDList.push_back(pResObject->GetSubObjectByIndex(i)->GetID());
	}
	else
		oi.wSubObjectCount = 0;

	long lg;
	if (!pResObject->GetPropertyLong("MaxID", lg))
		lg = 0;
	bt.Push(dt_int, &lg, 32);

	oi.wSize = bt.GetDataLength();
	oi.wOffset = oi.wSize;

	// д�ṹ
	fwrite(&oi, 1, sizeof(BINOBJECTINFO), fp);
	// д�Ӷ����б�
	if (!vSubObjectIDList.empty())
		fwrite(&vSubObjectIDList[0], 1, vSubObjectIDList.size()*sizeof(DWORD),fp);
	// д�û�����
	if (bt.GetDataLength())
	{
		//_data_xor((BYTE*)bt.GetDataBuffer(), bt.GetDataLength());
		fwrite(bt.GetDataBuffer(), 1, bt.GetDataLength(), fp);
	}

	ID_OFFSET idmap;
	idmap.id = 0;
	idmap.offset = nFilePos;
	idOffsetTable.push_back(idmap);
	nFilePos += sizeof(BINOBJECTINFO) + 
		oi.wSubObjectCount*sizeof(DWORD) + oi.cbUnFixCount*sizeof(WORD) + oi.wSize;
	return TRUE;
}
//------------------------------------------------------------------------
// ��õ�ǰ���ID��
UINT CResScript::GetMaxID()
{
	long lCurMaxID;
	m_pResObjectRoot->GetPropertyLong("MaxID", lCurMaxID);
	return lCurMaxID;
}
//------------------------------------------------------------------------
// ����res�ļ�,����ָ��ID�Ķ���Ľű�
BOOL CResScript::ParseObjectServer(UINT uID, char* pData)
{
	CResDecoder ResDecoder(this, m_pResObjectRoot);

	if (!ResDecoder.ParseObjectServer(uID, pData))
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
// ��ĳ���ַ�����������
IResObject* CResScript::ParseObjectClient(char* pData, int nSize)
{
	CResDecoder ResDecoder(this, m_pResObjectRoot);

	IResObject* pResObject = ResDecoder.ParseObjectClient(pData, nSize);
	Assert(pResObject);
	return pResObject;
}
//------------------------------------------------------------------------
// ���õ�ǰ���ID��
UINT CResScript::NextMaxID()
{
	UINT uID = GetMaxID();
	uID ++;
	Assert(m_pResObjectRoot != NULL);
	m_pResObjectRoot->SetPropertyLong("MaxID", uID);
	return uID;
}
//------------------------------------------------------------------------
void CResScript::Empty()
{
	SAFE_RELEASE(m_pResObjectRoot);

	m_nCount = 0;

	if (m_bReleaseFileObject)
	{
		SAFE_RELEASE(m_pFileObject);
	}

	if (!m_HashMap.empty())
		m_HashMap.clear();
}
//------------------------------------------------------------------------
#ifdef _DEBUG
BOOL CResScript::Dump()
{
	Assert(m_pResObjectRoot);
	DebugMsgLn(L"\r\n------------- ��ӡ�����ű���Ϣ��ʼ -------------");
	DebugMsgLn(L"���ű���Ϣ���ű��ļ� = \"%s\" �������� = %d", this->GetScriptFileName(), this->GetObjectCount());
	m_pResObjectRoot->Dump();
	DebugMsgLn(L"------------- ��ӡ�����ű���Ϣ���� -------------\r\n");
	return TRUE;
}
#endif
//------------------------------------------------------------------------