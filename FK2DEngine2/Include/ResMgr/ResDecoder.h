/**
*	created:		2012-6-18   20:18
*	filename: 		ResDecoder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// ��ṹ
typedef struct _MACRO
{
public:
	string Name;
	string Value;
public:
	_MACRO()	{Name.clear();Value.clear();}
	_MACRO(string name, string value)	{Name = name; Value = value;}
	~_MACRO()	{Name.clear();Value.clear();}
} MACRO;
//------------------------------------------------------------------------
// ��Դ������
class CResDecoder
{
private:
	CResScript*			m_pResScript;		// ��Դ�ű�ָ��
	char*				m_pScriptBuffer;	// ��Դ�ű�����
	char*				m_Pos;				// ��Դ�ű��ļ�ָ��
	vector<MACRO>		m_Macro;			// ������

public:
	CResDecoder(CResScript* pResScript, CResObject* pResObjectRoot);
	virtual ~CResDecoder();

	BOOL ParseFile(IFileObject* pFileObject, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL);
	BOOL ParseToBuffer(BOOL bIgnoreID = FALSE);
	LINETYPE GetLineType(string& strLine);
	inline char* GetStringLine(string& str);

	BOOL ParseObjectServer(UINT uID, char* pData);
	IResObject* ParseObjectClient(char* pData, int nSize);

	void RecordMacro(string strMacroName, string strValue);
	BOOL HandleMacro(string& strLine);
};
//------------------------------------------------------------------------