/**
*	created:		2012-6-18   20:27
*	filename: 		ResDecoder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../Include/ResMgr/ResMgrCommon.h"
#include "../../Include/ResMgr/ResObject.h"
#include "../../Include/ResMgr/ResScript.h"
#include "../../Include/ResMgr/ResDecoder.h"
#include <sys/stat.h>
#include "../../Include/ResMgr/Encrypt.h"
//------------------------------------------------------------------------
// ���캯��
CResDecoder::CResDecoder(CResScript* pResScript, CResObject* pResObjectRoot)
{
	m_pResScript = pResScript;
	m_pScriptBuffer = NULL;
	m_Pos = NULL;
}
//------------------------------------------------------------------------
// ��������
CResDecoder::~CResDecoder()
{
	// �ͷź���ռ��Դ
	if (!m_Macro.empty())
		m_Macro.clear();
}
//------------------------------------------------------------------------
// ������Դ�ű��ļ�
BOOL CResDecoder::ParseFile(IFileObject* pFileObject, BOOL bIgnoreID, BYTE* pKey)
{
	// ȷ���Ƿ��ȡ�����ڶ���ָ��
	Assert(pFileObject && m_pResScript);

	// ���ļ�
	if (!pFileObject->Open())
	{
		FKResTraceColorLn( MY_TRACE_COLOR, 
			L"ERROR: CResDecoder::ParseFile() catch error: can't open file[\"%s\"]", pFileObject->GetFilePath());
		return FALSE;
	}
	
	// ����ļ���С
	int uFileLen = pFileObject->GetLength();
	
	// �����ڴ�
	m_pScriptBuffer = new char[uFileLen + 3];
	Assert(m_pScriptBuffer);

	// ����
	pFileObject->SeekToBegin();
	pFileObject->Read(m_pScriptBuffer, uFileLen);
	pFileObject->Close();

	if (pKey)
	{
		CEncrypt en;
		en.Decrypt((BYTE*)m_pScriptBuffer, uFileLen + 3, pKey);
	}
	
	// �Զ�����һ���س����з���������һ���ַ����Ķ�ȡ
	m_pScriptBuffer[uFileLen] = '\r';
	m_pScriptBuffer[uFileLen+1] = '\n';
	m_pScriptBuffer[uFileLen+2] = '\0';
	m_Pos = m_pScriptBuffer;
	if (!ParseToBuffer(bIgnoreID))
	{
		delete [] m_pScriptBuffer;
		m_pScriptBuffer = NULL;
		return FALSE;
	}
	
	// ��ʱ�����ͷţ���ռ��
	// delete [] m_pScriptBuffer;
	m_pScriptBuffer = NULL;

	return TRUE;
}
//------------------------------------------------------------------------
// ���ļ������л�ȡһ���ı�
inline char* CResDecoder::GetStringLine(string& str)
{
	int nLen = 0;
	if (*m_Pos == 0) return NULL;
	while (*m_Pos != 13)
	{
		m_Pos ++;
		nLen ++;
	}
	*m_Pos = 10;
	str.clear();
	str.resize(nLen + 1);
	memcpy(&str[0], m_Pos - nLen, nLen + 1);
	m_Pos += 2;
	return (m_Pos - nLen - 2);
}
//------------------------------------------------------------------------
// ������������,���ɹ����ش����Ľű����ȣ����򷵻�0
BOOL CResDecoder::ParseToBuffer(BOOL bIgnoreID)
{
	int nLineNum = 0;				// ����
	string strLine;					// �л���
	int nDepth = 0, oldDepth = 0;	// ��������ȼ���
	string strName, strType, strID; // ��������֡����͡�ID����
	UINT uID = 0;
	CResObject* obj = static_cast<CResObject*>(m_pResScript->GetResObjectRoot());
	Assert(obj);
	UINT uParentID = obj->GetID();

    // һ��һ�еĶ�
	while(GetStringLine(strLine))
    {
		nLineNum++;

		switch(GetLineType(strLine))
		{
		case LT_UNDEFINE:
			FKResTraceColorLn(MY_TRACE_COLOR, L"�� %d �г��ֲ��ɽ����������﷨����\n\n%s", nLineNum, strLine.c_str());
			return FALSE;
		case LT_EMPTY:
		case LT_COMMENT:
			break;
		case LT_MACRO:
		{
			// ����ȥ��ÿ�к����ע��
			int nLocate = strLine.find_last_of("//");
			if (nLocate != -1)
				_left(strLine, nLocate - 1);

			_trim(strLine);
			_erase_left(strLine, 7); // 7 = #define
			_trim(strLine);
			int pos = _find_char(strLine, ' ');
			if (pos == 0)			// ������
				pos = _find_char(strLine, '\t');
			if (pos == 0) 
			{
				TraceLn("�� %d �г��ֲ��ɽ����ĺ궨�壺\n\n%s", nLineNum, strLine.c_str());
				return FALSE;
			}
			string strMacroName = strLine;
			string strValue = strLine;
			_left(strMacroName, pos - 1);
			_trim(strMacroName);
			_erase_left(strValue, pos);
			_trim(strValue);
			RecordMacro(strMacroName, strValue);
		}
		break;

		// ��¼����������������ID
		case LT_OBJECT:
			{
				HandleMacro(strLine);
				_trim(strLine);
				_erase_left(strLine, 6);

				int pos = _find_char(strLine, ':');
				int pos1 = _find_char(strLine, '=');
				strName.clear();

				if (pos) // ������
				{
					strName = strLine;
					_left(strName, pos - 1);
					_trim(strName);
					if (pos1) // ��ID
					{
						strType = strLine;
						_mid(strType, pos, pos1 - 2);
						_trim(strType);

						if (bIgnoreID)
							uID = 0;
						else
						{
							strID = strLine;
							_erase_left(strID, pos1);
							_trim(strID);
							uID = atoi(strID.c_str());
						}
					}
					else // ��ID
					{
						uID = 0;
						strType = strLine;
						_erase_left(strType, pos);
						_trim(strType);
					}
				}
				else // ������
				{
					strType.clear();
					if (pos1) // ��ID
					{
						strName = strLine;
						_left(strName, pos1 - 1);
						_trim(strName);

						if (bIgnoreID)
							uID = 0;
						else
						{
							strID = strLine;
							_erase_left(strID, pos1);
							_trim(strID);
							uID = atoi(strID.c_str());
						}
					}
					else // ��ID
					{
						strName = strLine;
						_trim(strName);
						uID = 0;
					}
				}
			}
			break;

		// newһ������
		case LT_LBRACKET:
			{
				nDepth ++;

				if (nDepth > oldDepth)			// ��һ���ӽڵ�
					uParentID = obj->GetID();

				// �½�����
				obj = new CResObject(m_pResScript);
				Assert(obj);
				obj->SetObjectInfo(strName.c_str(), strType.c_str());
				obj->SetParentID(uParentID);
				m_pResScript->AddToHashMap(uID, static_cast<IResObject*>(obj));
				
				IResObject* pParentObject = m_pResScript->GetResObject(uParentID);
				Assert(pParentObject);
				pParentObject->AddSubObject(static_cast<IResObject*>(obj));

				oldDepth = nDepth;
			}
			break;

		// ����
		case LT_RBRACKET:
			{
				nDepth --;
				obj = static_cast<CResObject*>(obj->GetParentObject());
				uParentID = obj->GetID();
				oldDepth = nDepth + 1;
			}
			break;

		// ����ǰ�����������
		case LT_PROPERTY:
			{
				HandleMacro(strLine);
				int pos = _find_char(strLine, '=');
				string strPropName = strLine;
				string strValue = strLine;
				_left(strPropName, pos - 1);
				_erase_left(strValue, pos);
				_trim(strPropName);
				_trim(strValue);
				Assert( obj->SetPropertyString(strPropName.c_str(), &strValue[0]) );
			}
			break;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// ����ÿһ�е��������,���޸�������
LINETYPE CResDecoder::GetLineType(string& strLine)
{
    char CurChar;
    bool bIsValidChar = false;
	int  k = 0;
    while (CurChar = strLine[k++])
    {
		switch(CurChar)
		{
		case '/':		// ��ע����("//")
			if (!bIsValidChar && strLine[k] == '/')
				return LT_COMMENT;
			else
				bIsValidChar = true;
			break;

		case ' ':		// �ǿո��TAB��������֮
		case '\t':
			break;

		case '\r':		// �ǻس���
		case '\n':		// �ǻ��з�
			if (!bIsValidChar)
				return LT_EMPTY;
			break;

		case '#':		// �Ǻ궨��(#define)
			if (!bIsValidChar)
				return LT_MACRO;
			break;

		case '{':		// ��{
			if (!bIsValidChar)
				return LT_LBRACKET;
			break;

		case '}':		// ��}
			if (!bIsValidChar)
				return LT_RBRACKET;
			break;

		case '=':		// ��}
			if (bIsValidChar)
				return LT_PROPERTY;
			break;

		case 'O':		// ���ܻ���Object
		case 'o':
			if (!bIsValidChar && _find_string_no_case(strLine, "object"))
				return LT_OBJECT;
			bIsValidChar = true;
			break;

		default: // �����ֻ򲻿�ʶ������
			bIsValidChar = true;
			break;
		}
	}

	return LT_UNDEFINE;
}
//------------------------------------------------------------------------
// ¼�ƺ�
void CResDecoder::RecordMacro(string strMacroName, string strValue)
{
	MACRO macro;
	macro.Name = strMacroName;
	macro.Value = strValue;
	m_Macro.push_back(macro);
}
//------------------------------------------------------------------------
// �����(���滻)
BOOL CResDecoder::HandleMacro(string& strLine)
{
	// ���滻
	if (m_Macro.empty())
		return TRUE;

	vector<MACRO>::iterator it;
	for (it = m_Macro.begin(); it != m_Macro.end(); it++)
	{
		MACRO  mac = *it;
		int pos = 1;
		while(pos)
		{
			pos = _find_string(strLine, mac.Name);
			if(!pos)
				break;

			strLine.replace(strLine.begin() + pos - 1, 
				strLine.begin() + pos -1 + mac.Name.size(),
							mac.Value.c_str());
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// ����res�ļ�,����ָ��ID�Ķ���Ľű�
BOOL CResDecoder::ParseObjectServer(UINT uID, char* pData)
{
	char buf[MAX_SCRIPT_LINE_LEN];
	BOOL bFlags = FALSE;
	BOOL bHaveRightBracket = FALSE;
	int nLineNum = 0;
	string strLine, strID;
	char strTmp[64];
	memset( strTmp, 0, sizeof(strTmp) );
	_itoa( nLineNum, strTmp, 10 );
	strID = string(strTmp);

	Assert(pData);
	
	IFileObject* pFileObject = _create_fileobject(m_pResScript->GetScriptFileName());
	Assert(pFileObject);
	pFileObject->Open("r");

	while (pFileObject->ReadString(buf, MAX_SCRIPT_LINE_LEN))
	{
		strLine = buf;
		nLineNum++;
		switch(GetLineType(strLine))
		{
		case LT_UNDEFINE:// δ��������
			TraceLn("�� %d �г��ֲ��ɽ����������﷨����\n\n%s", nLineNum, strLine.c_str());
			*pData = 0;
			return FALSE;
		case LT_EMPTY:
		case LT_COMMENT:
			continue;
			break;
		case LT_MACRO:
		{
			_trim(strLine);
			_erase_left(strLine, 7); // 7 = #define
			_trim(strLine);
			int pos = _find_char(strLine, ' ');
			if (pos == 0) // not exist!
				pos = _find_char(strLine, '\t');
			if (pos == 0) 
			{
				TraceLn("�� %d �г��ֲ��ɽ����ĺ궨�壺\n\n%s", nLineNum, strLine.c_str());
				*pData = 0;
				return FALSE;
			}
			string strMacroName = strLine;
			string strValue = strLine;
			_left(strMacroName, pos - 1);
			_trim(strMacroName);
			_erase_left(strValue, pos);
			_trim(strValue);
			RecordMacro(strMacroName, strValue);
		}
		break;

		case LT_OBJECT:// object ...
			HandleMacro(strLine);
			if (bFlags == FALSE)
			{
				if (_find_string(strLine, strID)) // ����ҵ���?�Ǻ�
				{
					// ���Ҫ��10����������100��?
					int pos = _find_char(strLine, '=');
					if (pos)
					{
						string strID1 = strLine;
						_erase_left(strID1, pos);
						_trim(strID1);
						UINT uID1 = atoi(strID1.c_str());
						if (uID1 != uID)
							continue;
					}

					bFlags = TRUE;
					if (*pData != '\0')
						*pData = '\0';
				}
			}
			else
			{
				if (!bHaveRightBracket)
				{
					_trim_left(strLine);
					lstrcatA(pData, "}\n");
				}

				SAFE_RELEASE(pFileObject);
				return TRUE;
			}
			break;

		case LT_LBRACKET: // "{"
			break;
		case LT_RBRACKET: // "}"
			if (bFlags)
			{
				bHaveRightBracket = TRUE;
				_trim_left(strLine);
				lstrcatA(pData, strLine.c_str());

				SAFE_RELEASE(pFileObject);
				return TRUE;
			}
			break;
		case LT_PROPERTY:
			HandleMacro(strLine);
			break;
		}

		if (bFlags)
		{
			_trim_left(strLine);
			lstrcatA(pData, strLine.c_str());
		}
	}

	// û��!
	pData[0] = '\0';
	SAFE_RELEASE(pFileObject);
	return FALSE;
}
//------------------------------------------------------------------------
// ��ĳ���ַ�����������
IResObject* CResDecoder::ParseObjectClient(char* pData, int nSize)
{
	string strLine, strName, strType, strID;
	CResObject* obj = NULL;
	UINT uID = 0;
	string buf = pData;
	int nPos = _find_char(buf, '\n');
	while(nPos)
	{
		strLine.resize(nPos);
		int nNum = buf.copy(&strLine[0], nPos);

		switch (GetLineType(strLine))
		{
		case LT_OBJECT:
			{
				_trim(strLine);
				_erase_left(strLine, 6); // erase "Object"

				int pos = _find_char(strLine, ':');
				int pos1 = _find_char(strLine, '=');

				if (pos) // ������
				{
					strName = strLine;
					_left(strName, pos - 1);
					_trim(strName);
					if (pos1) // ��ID
					{
						strType = strLine;
						_mid(strType, pos, pos1 - 2);
						_trim(strType);

						strID = strLine;
						_erase_left(strID, pos1);
						_trim(strID);

					}
					else // ��ID
					{
						uID = 0;
						strType = strLine;
						_erase_left(strType, pos);
						_trim(strType);
					}
				}
				else // ������
				{
					strType.clear();
					if (pos1) // ��ID
					{
						strName = strLine;
						_left(strName, pos1 - 1);
						_trim(strName);

						strID = strLine;
						_erase_left(strID, pos1);
						_trim(strID);
						uID = atoi(strID.c_str());
					}
					else // ��ID
					{
						strName = strLine;
						_trim(strName);
						uID = 0;
					}
				}
			}
			break;

		case LT_LBRACKET:
			{
				// �½�����
				obj = new CResObject();
				Assert(obj != NULL);
				obj->SetObjectInfo(strName.c_str(), strType.c_str());
				obj->SetID(uID);
			}
			break;
		case LT_RBRACKET:
			return static_cast<IResObject*>(obj);
		case LT_PROPERTY:
			{
				int pos = _find_char(strLine, '=');
				string strPropName = strLine;
				string strValue = strLine;
				_left(strPropName, pos - 1);
				_erase_left(strValue, pos);
				_trim(strPropName);
				_trim(strValue);
				Assert( obj->SetPropertyString(strPropName.c_str(), &strValue[0]) );
			}
			break;
		}

	_erase_left(buf, nPos);
	nPos = _find_char(buf, '\n');
	}
	
	return static_cast<IResObject*>(obj);
}
//------------------------------------------------------------------------