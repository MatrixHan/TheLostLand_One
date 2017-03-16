/**
*	created:		2012-6-18   22:25
*	filename: 		ResObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../Include/ResMgr/ResMgrCommon.h"
#include "../../Include/ResMgr/ResScript.h"
#include "../../Include/ResMgr/ResObject.h"
//------------------------------------------------------------------------
CResObject::CResObject()
{
	if (!m_vProp.empty())
		m_vProp.clear();
	if (!m_vID.empty())
		m_vID.clear();
	m_pResScript = NULL;
}
//------------------------------------------------------------------------
CResObject::CResObject(CResScript* pResScript)
{
	if (!m_vProp.empty())
		m_vProp.clear();
	if (!m_vID.empty())
		m_vID.clear();
	m_pResScript = pResScript;
}
//------------------------------------------------------------------------
CResObject::~CResObject()
{
}
//------------------------------------------------------------------------
// ���ö�����Ϣ
void CResObject::SetObjectInfo(LPCSTR szObjectName, LPCSTR szTypeName)
{
	if(szObjectName != NULL && *szObjectName != 0)
		m_ResObjectInfo.strName = szObjectName;
	if(szTypeName != NULL && *szTypeName != 0)
		m_ResObjectInfo.strType = szTypeName;
}
//------------------------------------------------------------------------
// ��������
BOOL CResObject::AddProperty(LPCSTR szPropertyName, LPCSTR szPropertyValue)
{
	if (FindProperty(szPropertyName) >= 0) // exist!
	{
		FKResTraceColorLn(MY_TRACE_COLOR, 
			L"���� CResObject::AddProperty() �����쳣������ %s �����ڣ�", szPropertyName);
		return FALSE;
	}

	return SetProperty(szPropertyName, szPropertyValue);
}
//------------------------------------------------------------------------
// ɾ������
BOOL CResObject::EraseProperty(LPCSTR szPropertyName)
{
	int i = FindProperty(szPropertyName);
	if (i >= 0)
	{
		m_vProp.erase(m_vProp.begin() + (i<<1)); // ɾ��it��it+1 �����it
		m_vProp.erase(m_vProp.begin() + (i<<1));
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
// ����������
BOOL CResObject::SetPropertyName(LPCSTR szOldPropertyName, LPCSTR szNewPropertyName)
{
	int i = FindProperty(szOldPropertyName);
	if (i >= 0)
		return ModifyString((i<<1), szNewPropertyName);

	return FALSE;
}
//------------------------------------------------------------------------
// ��ø�����
IResObject* CResObject::GetParentObject()
{
	if (!m_pResScript) return NULL;
	return m_pResScript->GetResObject(m_ResObjectInfo.uParentID);
}
//------------------------------------------------------------------------
// ��ȡ�ַ�������
LPCSTR CResObject::GetPropertyString(LPCSTR szPropertyName)
{
	int i = FindProperty(szPropertyName);
	if (i >= 0)
		return (m_vProp.begin() + (i<<1) + 1)->c_str();
	
	// δ�ҵ�ָ��������
	return NULL;
}
//------------------------------------------------------------------------
// ��ȡ��������
BOOL CResObject::GetPropertyLong(LPCSTR szPropertyName, long& nReturn, long nDefault)
{
	int i = FindProperty(szPropertyName);
	if (i >= 0)
	{
		nReturn = atol((m_vProp.begin() + (i<<1) + 1)->c_str());
		return TRUE;
	}

	// δ�ҵ�ָ��������
	nReturn = nDefault;
	return FALSE;
}
//------------------------------------------------------------------------
// ��ȡʵ������
BOOL CResObject::GetPropertyFloat(LPCSTR szPropertyName, double& dlReturn, double dlDefault)
{
	int i = FindProperty(szPropertyName);
	if (i >= 0)
	{
		dlReturn = atof((m_vProp.begin() + (i<<1) + 1)->c_str());
		return TRUE;
	}

	// δ�ҵ�ָ��������
	dlReturn = dlDefault;
	return FALSE;
}
//------------------------------------------------------------------------
// �����ַ�������
BOOL CResObject::SetPropertyString(LPCSTR szPropertyName, LPCSTR szSetString)
{
	int i = FindProperty(szPropertyName);
	if (i >= 0)
		return ModifyString((i<<1) + 1, szSetString);

	return SetProperty(szPropertyName, szSetString);
}
//------------------------------------------------------------------------
// ������������
BOOL CResObject::SetPropertyLong(LPCSTR szPropertyName, long nSet)
{
	string strBuffer(16, 0); // ��ֹ�ռ��С
	char szTmp[16];
	memset( szTmp, 0, sizeof(szTmp) );
	_ltoa(nSet, szTmp, 10);
	strBuffer = string( szTmp );
	int i = FindProperty(szPropertyName);
	if (i >= 0)
		return ModifyString((i<<1) + 1, strBuffer.c_str());

	return SetProperty(szPropertyName, strBuffer.c_str());
}
//------------------------------------------------------------------------
// ����ʵ������
BOOL CResObject::SetPropertyFloat(LPCSTR szPropertyName, double dlSet)
{
	string strBuffer(32, 0);
	char szTmp[32];
	memset( szTmp, 0, sizeof(szTmp) );
   _gcvt(dlSet, 30, szTmp);
   strBuffer = string( szTmp );
	int i = FindProperty(szPropertyName);
	if (i >= 0)
		return ModifyString((i<<1) + 1, strBuffer.c_str());

	return SetProperty(szPropertyName, strBuffer.c_str());
}
//------------------------------------------------------------------------
// �õ���������
int CResObject::GetArrayLong (LPCSTR szArrayName, long* lpData, int nCount)
{
	char val[MAX_SCRIPT_LINE_LEN];
	int i = 0;
	char * start, * str;

	int nIndex = FindProperty(szArrayName);
	if (nIndex < 0) 
		return 0;
	start = str = (char*)(m_vProp.begin()+ (nIndex<<1) + 1)->c_str();

	if (*str == 0) return 0;
	if (lpData == NULL) // �����鳤��
	{
		while (*str != 0)
		{
			if (*str == ',')
				i ++;
			str ++;
		}
		i ++;
		return i;
	}
	else
	{
		while (*str != 0)
		{
			if (*str == ',')
			{
				memcpy(val, start, str - start);
				val[str - start] = 0;
				start = str + 1;
				lpData[i] = atol(val);
				i ++;
				if (nCount != 0 && nCount <= i)
					return i;
			}
			str ++;
		}
		memcpy(val, start, str - start);
		val[str - start] = 0;
		lpData[i] = atol(val);
		i ++;
		return i;
	}
	return 0;
}
//------------------------------------------------------------------------
// �õ�ʵ������
int CResObject::GetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount)
{
	char val[MAX_SCRIPT_LINE_LEN];
	int i = 0;
	char * start, * str;

	int nIndex = FindProperty(szArrayName);
	if (nIndex < 0) 
		return 0;
	start = str = (char*)(m_vProp.begin()+ (nIndex<<1) + 1)->c_str();

	if (*str == 0) return 0;
	if (lpData == NULL) // �����鳤��
	{
		while (*str != 0)
		{
			if (*str == ',')
				i ++;
			str ++;
		}
		i ++;
		return i;
	}
	else
	{
		while (*str != 0)
		{
			if (*str == ',')
			{
				memcpy(val, start, str - start);
				val[str - start] = 0;
				start = str + 1;
				lpData[i] = atof(val);
				i ++;
				if (nCount != 0 && nCount <= i)
					return i;
			}
			str ++;
		}
		memcpy(val, start, str - start);
		val[str - start] = 0;
		lpData[i] = atof(val);
		i ++;
		return i;
	}
	return 0;
}
//------------------------------------------------------------------------
// �õ��ֽ�������
int CResObject::GetArrayByte(LPCSTR szArrayName, BYTE* lpData, int nCount)
{
	char val[MAX_SCRIPT_LINE_LEN];
	int i = 0;
	char * start, * str;

	//GetPropertyString(szArrayName, buf); // �õ���������
	int nIndex = FindProperty(szArrayName);
	if (nIndex < 0) 
		return 0;
	start = str = (char*)(m_vProp.begin()+ (nIndex<<1) + 1)->c_str(); // buf;

	if (*str == 0) return 0;
	if (lpData == NULL) // �����鳤��
	{
		while (*str != 0)
		{
			if (*str == ',')
				i ++;
			str ++;
		}
		i ++;
		return i;
	}
	else
	{
		while (*str != 0)
		{
			if (*str == ',')
			{
				memcpy(val, start, str - start);
				val[str - start] = 0;
				start = str + 1;
				lpData[i] = (BYTE)atoi(val);
				i ++;
				if (nCount != 0 && nCount <= i)
					return i;
			}
			str ++;
		}
		memcpy(val, start, str - start);
		val[str - start] = 0;
		lpData[i] = (BYTE)atoi(val);
		i ++;
		return i;
	}
	return 0;
}
//------------------------------------------------------------------------
// ������������
BOOL CResObject::SetArrayLong (LPCSTR szArrayName, long* lpData, int nCount)
{
	char buf[MAX_SCRIPT_LINE_LEN];
	char val[64];
	char* start = buf;
	int nEachLength = 0;

	Assert(nCount > 0 && lpData != NULL);

	for (int i = 0; i < nCount; i ++)
	{
		_ltoa(lpData[i], val, 10);
		nEachLength = lstrlenA(val);
		memcpy(start, val, nEachLength);
		start += nEachLength + 1;
		*(start - 1) = ',';
#ifdef _DEBUG
		if (start - buf > MAX_SCRIPT_LINE_LEN)
		{
			FKResTraceColorLn(MY_TRACE_COLOR, L"���� CResObject::SetArrayLong() �����쳣��������������Ҫ����Ϊ %d �ֽ���", MAX_SCRIPT_LINE_LEN);
			return FALSE;
		}
#endif
	}
	*(start - 1) = '\0';

	int m = FindProperty(szArrayName);
	if (m >= 0)
		return ModifyString((m<<1) + 1, buf);

	return SetProperty(szArrayName, buf);
}
//------------------------------------------------------------------------
// ����ʵ������
BOOL CResObject::SetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount)
{
	char buf[MAX_SCRIPT_LINE_LEN];
	char val[64];
	char* start = buf;
	int nEachLength = 0;

	Assert(nCount > 0 && lpData != NULL);

	for (int i = 0; i < nCount; i ++)
	{
		_gcvt(lpData[i], 30, val);
		nEachLength = lstrlenA(val);
		memcpy(start, val, nEachLength);
		start += nEachLength + 1;
		*(start - 1) = ',';
#ifdef _DEBUG
		if (start - buf > MAX_SCRIPT_LINE_LEN)
		{
			FKResTraceColorLn(MY_TRACE_COLOR, L"���� CResObject::SetArrayFloat() �����쳣��������������Ҫ����Ϊ %d �ֽ���", MAX_SCRIPT_LINE_LEN);
			return FALSE;
		}
#endif
	}
	*(start - 1) = 0;

	int m = FindProperty(szArrayName);
	if (m >= 0) 
		return ModifyString((m<<1) + 1, buf);

	return SetProperty(szArrayName, buf);
}
//------------------------------------------------------------------------
// �����ֽ�������
BOOL CResObject::SetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount)
{
	char buf[MAX_SCRIPT_LINE_LEN];
	char val[64];
	char* start = buf;
	int nEachLength = 0;

	Assert(nCount > 0 && lpData != NULL);

	for (int i = 0; i < nCount; i ++)
	{
		_itoa(lpData[i], val, 10);
		nEachLength = lstrlenA(val);
		memcpy(start, val, nEachLength);
		start += nEachLength + 1;
		*(start - 1) = ',';
#ifdef _DEBUG
		if (start - buf > MAX_SCRIPT_LINE_LEN)
		{
			FKResTraceColorLn(MY_TRACE_COLOR, L"���� CResObject::SetArrayFloat() �����쳣��������������Ҫ����Ϊ %d �ֽ���", MAX_SCRIPT_LINE_LEN);
			return FALSE;
		}
#endif
	}
	*(start - 1) = 0;

	int m = FindProperty(szArrayName);
	if (m >= 0) // exist!
		return ModifyString((m<<1) + 1, buf);

	return SetProperty(szArrayName, buf);
}
//------------------------------------------------------------------------
// ��ȡ���Եĸ���
int CResObject::GetPropertyCount()
{
	return m_vProp.size()>>1;
}
//------------------------------------------------------------------------
int CResObject::GetSubObjectCount()
{
	return m_vID.size();
}
//------------------------------------------------------------------------
IResObject* CResObject::GetResObject(UINT uID)
{
	return m_pResScript->GetResObject(uID);
}
//------------------------------------------------------------------------
IResObject* CResObject::GetSubObjectByName(LPCSTR szObjectName)
{
	IResObject* pResObject = NULL;
	if (szObjectName == NULL) // β����
		return GetSubObjectByIndex(m_vID.size() - 1);
	else
	{
		for (int i=0; i<m_vID.size(); i++)
		{
			pResObject = GetSubObjectByIndex(i);
			Assert(pResObject != NULL);
			if (_stricmp(pResObject->GetObjectName(), szObjectName) == 0) // find!
				return pResObject;
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
IResObject* CResObject::GetSubObjectByIndex(int nIndex)
{
	Assert(nIndex >= 0 && nIndex < m_vID.size());
	Assert(m_pResScript);
	return m_pResScript->GetResObject(m_vID[nIndex]);
}
//------------------------------------------------------------------------
IResObject* CResObject::CreateSubObject(int nIndex/*-1*/)
{
	Assert(nIndex >= -1 && nIndex <= (int)m_vID.size());
	Assert(m_pResScript);
	if (nIndex >= -1 && nIndex <= (int)m_vID.size())
	{
		CResObject* pResObject = new CResObject(m_pResScript);
		Assert(pResObject != NULL);
		pResObject->SetParentID(GetID());
		m_pResScript->AddToHashMap(0, static_cast<IResObject*>(pResObject));
		if (nIndex <= -1 || nIndex >= (int)m_vID.size())
			m_vID.push_back(pResObject->GetID());
		else
		{
			vector<UINT>::iterator Ite = m_vID.begin() + nIndex;
			m_vID.insert( Ite, pResObject->GetID());
		}
		return static_cast<IResObject*>(pResObject);
	}
	return NULL;
}
//------------------------------------------------------------------------
BOOL CResObject::AddSubObject(IResObject* pResObject)
{
	Assert(pResObject != NULL);

	UINT uID = pResObject->GetID();
	if (uID == 0)
		return FALSE;

	for (int i=0; i<m_vID.size(); i++)
	{
		if (m_vID[i] == uID)
		{
			FKResTraceColorLn(MY_TRACE_COLOR, L"ID(%d) same!", uID);
			return FALSE;
		}
	}
	static_cast<CResObject*>(pResObject)->SetParentID(GetID());
	m_vID.push_back(uID);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResObject::EraseSubObjectByName(LPCSTR szObjectName/* = NULL*/)
{
	IResObject* pResObject = NULL;
	if (szObjectName == NULL) // ɾ��β����
		return EraseSubObjectByIndex(m_vID.size() - 1);
	else
	{
		pResObject = GetSubObjectByName(szObjectName);
		if (pResObject)
			return pResObject->Release();
	}
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CResObject::EraseSubObjectByIndex(int nIndex)
{
	IResObject* pResObject = NULL;
	Assert(nIndex >= 0 && nIndex < m_vID.size());
	pResObject = GetResObject(m_vID[nIndex]);
	if (pResObject)
		return pResObject->Release();
	return FALSE;
}
//------------------------------------------------------------------------
// ��ձ�����
void CResObject::Empty()
{
	if (!m_vProp.empty())
		m_vProp.clear();
}
//------------------------------------------------------------------------
// �ͷ�(������ԡ���������)
BOOL CResObject::Release()
{
	// �ͷ��Ӷ���
	IResObject* pResObject = NULL;
	int nCount = m_vID.size();
	for(int i=0; i<nCount; i++)
	{
		pResObject = GetSubObjectByIndex(0);
		SAFE_RELEASE(pResObject);
	}

	// ȥ��ϣ��ע��
	if (m_pResScript)
		m_pResScript->EraseFromHashMap(GetID());

	// ɾ���������б�����ID��
	if (GetParentObject())
		(static_cast<CResObject*>(GetParentObject()))->RemoveID(GetID());
	// ɾ����������Ϣ
	Empty();

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
// ���ö��������
BOOL CResObject::SetProperty(LPCSTR szPropertyName,LPCSTR szString)
{
	if (szPropertyName == NULL)
		m_vProp.push_back("");
	else
		m_vProp.push_back(szPropertyName);

	if (szString == NULL)
		m_vProp.push_back("");
	else
		m_vProp.push_back(szString);

	return TRUE;
}
//------------------------------------------------------------------------
// �õ���nIndex��������
LPCSTR CResObject::GetPropertyName(int nIndex)
{
	Assert(nIndex >= 0 && nIndex < (m_vProp.size()>>1));

	vector<string>::iterator it = m_vProp.begin() + (nIndex<<1);
	return it->c_str();
}
//------------------------------------------------------------------------
// �õ���nIndex������ֵ
LPCSTR CResObject::GetPropertyValue(int nIndex)
{
	Assert(nIndex >= 0 && nIndex < (m_vProp.size()>>1));

	vector<string>::iterator it = m_vProp.begin() + (nIndex<<1) +1;
	return it->c_str();
}
//------------------------------------------------------------------------
// �������ԣ�������������ţ�û�鵽���أ�1
int CResObject::FindProperty(LPCSTR szPropertyName)
{
	if (szPropertyName == NULL)
		szPropertyName = "";
	vector<string>::iterator it;
	int i;
	for (i=0, it=m_vProp.begin(); it!=m_vProp.end(); it+=2, i++)
		if (_stricmp(it->c_str(), szPropertyName) == 0)
			return i;

	return -1;
}
//------------------------------------------------------------------------
// �޸�ĳ���ַ���
BOOL CResObject::ModifyString(int nIndex, LPCSTR szString)
{
	if (szString == NULL)
		return FALSE;
	
	m_vProp[nIndex] = szString;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResObject::RemoveID(UINT uID)
{
	if (m_vID.empty())
		return TRUE;
	for (int i=0; i<m_vID.size(); i++)
	{
		if (m_vID[i] == uID)
		{
			m_vID.erase(m_vID.begin() + i);
			return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------
IResObject* CResObject::Duplicate(BOOL bUseNewID)
{
	Assert(m_pResScript);
	CResObject* pResObject = new CResObject(m_pResScript);
	Assert(pResObject != NULL);
	pResObject->m_ResObjectInfo = m_ResObjectInfo;
	if (bUseNewID)
		pResObject->SetID(m_pResScript->NextMaxID());
	else
		pResObject->SetID(GetID());
	pResObject->m_vProp = m_vProp;
	return static_cast<IResObject*>(pResObject);
}
//------------------------------------------------------------------------
IResScript* CResObject::GetResScript()
{
	return static_cast<IResScript*>(m_pResScript);
}
//------------------------------------------------------------------------
#ifdef _DEBUG
BOOL CResObject::Dump()
{
	DebugMsgLn(L"__________________________________________");
	DebugMsgLn(L"��������Ϣ�� %s : %s = %d", GetObjectName(), GetTypeName(), GetID());
	DebugMsgLn(L"�����ԡ���Ŀ = %d", GetPropertyCount());
	for (int m=0; m<GetPropertyCount(); m++)
		DebugMsgLn(L"\t%s = %s", GetPropertyName(m), GetPropertyValue(m));
	DebugMsgLn(L"���Ӷ�����Ŀ = %d", GetSubObjectCount());
	for (int i=0; i<GetSubObjectCount(); i++)
		DebugMsgLn(L"\t��ID - ���֡�= %8d,   %s", GetSubObjectByIndex(i)->GetID(),GetSubObjectByIndex(i)->GetObjectName());

	IResObject* pSubObject;
	for (int k=0; k<GetSubObjectCount(); k++)
	{
		pSubObject = this->GetSubObjectByIndex(k);
		Assert(pSubObject);
		static_cast<CResObject*>(pSubObject)->Dump();
	}
	return TRUE;
}
#endif
//------------------------------------------------------------------------