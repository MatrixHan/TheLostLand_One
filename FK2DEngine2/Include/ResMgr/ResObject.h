/**
*	created:		2012-6-18   20:21
*	filename: 		ResObject
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CResObject : public IResObject
{
public:

private:
	RESOBJECTINFO	m_ResObjectInfo;	// ������Ϣ
	vector<string>	m_vProp;			// ��������
	vector<UINT>	m_vID;				// �Ӷ���ID�б�
public:
	CResScript*		m_pResScript;		// ����ͨ��ID������������

public:
	CResObject();
	CResObject(CResScript* pResScript);
	virtual ~CResObject();

	// IResObject
	// ������Ϣ���
	void	SetObjectInfo(LPCSTR szObjectName = NULL, LPCSTR szTypeName = NULL);
	LPCSTR	GetObjectName()	{return m_ResObjectInfo.strName.c_str();}
	LPCSTR	GetTypeName(){return m_ResObjectInfo.strType.c_str();}
	UINT	GetID()			{return m_ResObjectInfo.uID;}
	void Empty();
	BOOL Release();
	// ȫ�ֶ������
	IResObject* GetResObject(UINT uID);
	// ���������
	UINT	GetParentID()	{return m_ResObjectInfo.uParentID;}
	IResObject* GetParentObject();
	// �Ӷ������
	IResObject* CreateSubObject(int nIndex = -1);
	int GetSubObjectCount();
	IResObject* GetSubObjectByName(LPCSTR szObjectName = NULL);
	IResObject* GetSubObjectByIndex(int nIndex);
	BOOL AddSubObject(IResObject* pResObject);
	BOOL EraseSubObjectByName(LPCSTR szObjectName = NULL);
	BOOL EraseSubObjectByIndex(int nIndex);
	// �����������
	int  GetPropertyCount();
	LPCSTR GetPropertyName(int nIndex);
	LPCSTR GetPropertyValue(int nIndex);
	BOOL AddProperty(LPCSTR szPropertyName, LPCSTR szPropertyValue = NULL);
	BOOL EraseProperty(LPCSTR szPropertyName);
	BOOL SetPropertyName(LPCSTR szOldPropertyName, LPCSTR szNewPropertyName);
	LPCSTR GetPropertyString(LPCSTR szPropertyName);
	BOOL GetPropertyLong  (LPCSTR szPropertyName, long& nReturn, long nDefault = 0L);
	BOOL GetPropertyFloat (LPCSTR szPropertyName, double& dlReturn,	double dlDefault = 0.0);
	int GetArrayLong (LPCSTR szArrayName, long*   lpData, int nCount = 0);
	int GetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount = 0);
	int GetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount = 0);
	// ����...
	BOOL SetPropertyString(LPCSTR szPropertyName, LPCSTR szSetString);
	BOOL SetPropertyLong  (LPCSTR szPropertyName, long nSet);
	BOOL SetPropertyFloat (LPCSTR szPropertyName, double dlSet);
	BOOL SetArrayLong (LPCSTR szArrayName, long*   lpData, int nCount);
	BOOL SetArrayFloat(LPCSTR szArrayName, double* lpData, int nCount);
	BOOL SetArrayByte (LPCSTR szArrayName, BYTE*   lpData, int nCount);
	IResObject* Duplicate(BOOL bUseNewID = TRUE);
	IResScript* GetResScript();

	BOOL SetProperty(LPCSTR szPropertyName, LPCSTR szString = NULL);
	void SetParentID(UINT uID)	{ m_ResObjectInfo.uParentID=uID;}
	void SetID(UINT uID)	{ m_ResObjectInfo.uID = uID;}
	int	  FindProperty(LPCSTR szPropertyName);
	BOOL ModifyString(int nIndex, LPCSTR szString);
	BOOL ExistType(){return (!m_ResObjectInfo.strType.empty());}
	BOOL RemoveID(UINT uID);

#ifdef _DEBUG
	BOOL Dump();
#endif
};
//------------------------------------------------------------------------