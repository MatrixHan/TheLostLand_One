/**
*	created:		2012-6-18   19:42
*	filename: 		ResMgrCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <hash_map>
#include <queue>
#include <Mmsystem.h>
//------------------------------------------------------------------------
#include "../FKIEngine.h"
#include "../FKITrace.h"
#include "../IFKFileSystem.h"
#include "../FileSystem/FileSysCommon.h"
#include "../IFKResManager.h"
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
// ���ú궨��
#if !defined(ASSERT)
	#if defined(_DEBUG)
		#define ASSERT(f)			{if (!(f))__asm int 3}
	#else
		#define ASSERT(f)			((void*)0)
	#endif
#endif
//------------------------------------------------------------------------
#define FKResTraceColor		g_pEngineInstance->getTraceWin()->ThreadSafeDebugPrintColor
#define	FKResTraceColorLn	g_pEngineInstance->getTraceWin()->ThreadSafeDebugPrintColorLn
#define	DebugMsgLn			g_pEngineInstance->getTraceWin()->ThreadSafeDebugPrintLn
//------------------------------------------------------------------------
#ifndef	MY_TRACE_COLOR
	#define MY_TRACE_COLOR		RGB(146,0,123)
#endif
//------------------------------------------------------------------------
#ifdef TraceLn
	#undef TraceLn
#endif
#define TraceLn	DebugPrintLn
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		FKResTraceColor(MY_TRACE_COLOR, L"���� : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		FKResTraceColor(MY_TRACE_COLOR, L"���� : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,#Expression);\
		return 0;\
	}
#else
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		return 0;\
	}
#endif
//------------------------------------------------------------------------
class CResMgr;
class CResScript;
class CResObject;
class CResDecoder;
class CBitTool;
class CBinObject;
class CBinScript;
//------------------------------------------------------------------------
extern void*		_data_xor(BYTE* buf, int nLen);
extern IFileObject* _create_fileobject(LPCSTR szFileName);
// һЩ�й��ַ���������ȫ�ֺ���
extern int  		_find_string(string& str, string& str1, int nStart = 0);
extern int  		_find_string_no_case(string str, string str1, int nStart = 0);
extern int  		_find_char(string& str, char c, int nStart = 0);
extern int  		_find_char_no_case(string str, char c, int nStart = 0);
extern const char* 	_make_lower(string& str);
extern const char* 	_trim_left(string& str);
extern const char* 	_trim_right(string& str);
extern const char* 	_trim(string& str);
extern const char* 	_erase_left(string& str, int nSize);
extern const char* 	_erase_right(string& str, int nSize);
extern const char* 	_mid(string& str, int nStart, int nEnd);
extern const char* 	_mid_len(string& str, int nStart, int nLen);
extern const char* 	_left(string& str, int nSize);
extern const char* 	_right(string& str, int nSize);
extern const char* 	_left_c(string& str, char c);			// �任Ϊĳ���ַ���ߵĴ�
extern const char* 	_right_c(string& str, char c);			// �任Ϊĳ���ַ��ұߵĴ�
extern const char* 	_file_path(string& str);
extern const char* 	_file_name(string& str);
extern const char* 	_file_title(string& str);
extern const char* 	_file_ext(string& str);
//------------------------------------------------------------------------
// �ű�����е����Ͷ���
enum LINETYPE
{
	LT_UNDEFINE=0,	// ��ʾ������δ֪����Ϣ��
	LT_EMPTY,		// ��ʾ�����ǿ���
	LT_COMMENT,		// ��ʾ������ע����
	LT_MACRO,		// ��ʾ�궨��
	LT_LBRACKET,	// ��ʾ������������ţ���"{"
	LT_RBRACKET,	// ��ʾ�������Ҵ����ţ���"}"

	LT_OBJECT,		// ��ʾ�����Ƕ��������У���Object objname
	LT_PROPERTY,	// ��ʾ��������������
};
//------------------------------------------------------------------------
// ��Դ������Ϣ
typedef struct _RESOBJECTINFO
{
	string	strName;
	string	strType;
	UINT	uID;
	UINT	uParentID;
	_RESOBJECTINFO()		{strName.clear();strType.clear();uID=uParentID=0;}
	~_RESOBJECTINFO()		{strName.clear();strType.clear();}
} RESOBJECTINFO;
//------------------------------------------------------------------------
// ����ģ������֧�ֵ���������
enum DATATYPE
{
	dt_undefine=0,
	dt_bool,
	dt_char,
	dt_short,
	dt_int,
	dt_float,
	dt_byte,
	dt_word,
	dt_dword,
	dt_string
};
//------------------------------------------------------------------------
// ID��Offset��ӳ��
typedef struct _ID_OFFSET
{
	UINT id;
	DWORD offset;
} ID_OFFSET;
//------------------------------------------------------------------------
// ����ģ���е�������Ϣ
typedef struct _PROPINFO
{
	DATATYPE type;						// ����
	string	 propname;					// ������
	int		 length;					// ���Ե��򳤣���������Ϊ���鳤��
	bool	 isArray;					// �Ƿ�������

	_PROPINFO()						{length=0;type=dt_undefine;isArray=false;propname.clear();}
	void Release()					{delete this;}
	bool IsFixProperty()			{return ((length>0)&&(isArray==false));}		// �ǹ̶����ȵı�������
	bool IsFixArray()				{return ((length>0)&&(isArray==true));}			// �ǹ̶����ȵ���������
	bool IsUnFixProperty()			{return ((length==0)&&(isArray==true));}		// �ǲ������ȵ�����
	int  GetLength()				{return length;}
} PROPINFO;	
//------------------------------------------------------------------------
// ����ģ����Ϣ
typedef struct _TYPEINFO
{
	string				name;			// ������
	BYTE				cbID;			// ����ID
	vector<PROPINFO>	prop;			// ��������

	_TYPEINFO(){cbID=0;name.clear();prop.clear();}
	void Release()
	{
		if (!prop.empty()) prop.clear();
		delete this;
	}
	BYTE GetTypeID() {return cbID;}
	int GetFixPropertyLength()
	{
		int bitlen=0,bytelen=0;
		for(unsigned int i=0;i<prop.size();i++)
			if(prop[i].IsFixProperty())
				bitlen+=prop[i].GetLength();

		bytelen=bitlen>>3;
		if (bitlen&7)
			bytelen+=1;
		return bytelen;
	}
	int GetFixArrayLength()
	{
		static int bytes[] = {0,1,1,2,4,4,1,2,4,0};
		int arraylen=0;
		for(unsigned int i=0;i<prop.size();i++)
			if(prop[i].IsFixArray())
				arraylen+=prop[i].GetLength()*bytes[prop[i].type];
		return arraylen;
	}
	int GetUnFixPropertyCount()
	{
		int unfix_prop_count=0;
		for(unsigned int i=0;i<prop.size();i++)
			if(prop[i].IsUnFixProperty())
				unfix_prop_count++;
		return unfix_prop_count;
	}
} TYPEINFO;
//------------------------------------------------------------------------
// �����ƶ�����Ϣ
typedef struct _BINOBJECTINFO	
{
	BYTE	cbIdentify;		// ��ʶ
	BYTE	cbVersion;		// �汾��
	WORD	wSize;			// ���ݻ���Ĵ�С
	WORD	wOffset;		// ���������ݵ�ƫ��
	WORD	wSubObjectCount;// �Ӷ�����Ŀ( < 65535)
	BYTE	cbUnFixCount;	// ���������Ե���Ŀ

	_BINOBJECTINFO()				{memset(this,0,sizeof(_BINOBJECTINFO));cbIdentify=255;cbVersion=1;}
	int GetSize()					{return (int)wSize;}
	int GetUnFixPropertyOffset()	{return (int)wOffset;}
	int GetSubObjectCount()			{return (int)wSubObjectCount;}
	int GetUnFixPropertyCount()		{return (int)cbUnFixCount;}
	BOOL IsValid()					{return ((cbIdentify!=255)&&(cbVersion==1));}
} BINOBJECTINFO;
//------------------------------------------------------------------------
// �����ƶ����ļ�ͷ
typedef struct _BINOBJECTHEADER
{
	DWORD	dwIdentify;		// ��ʶ
	DWORD	dwObjectCount;	// ��������
	DWORD	dwMapOffset;	// ӳ���ƫ��
	_BINOBJECTHEADER()				{memset(this,0,sizeof(_BINOBJECTHEADER));dwIdentify= 'DHOB';}//'IBKF';}
	BOOL IsValid()					{return (dwIdentify=='DHOB');}//'IBKF');}
} BINOBJECTHEADER;
//------------------------------------------------------------------------