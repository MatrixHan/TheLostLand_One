/**
*	created:		2012-6-15   22:12
*	filename: 		FileSysCommon
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
#include <map>
#include "../IFKFileSystem.h"
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
class CFileSystem;
class CStdFileObject;
class CPackFileObject;
//------------------------------------------------------------------------
void DebugPrintLn(LPCSTR lpszFormat, ...);
//------------------------------------------------------------------------
#ifdef TraceLn
#undef TraceLn
#endif
#define TraceLn	DebugPrintLn
//------------------------------------------------------------------------
#if !defined(ASSERT)
#if defined(_DEBUG)
#define ASSERT(f) {if (!(f))__asm int 3}
#else
#define ASSERT(f)	 ((void*)0)
#endif
#endif
//------------------------------------------------------------------------
#define SAFE_DELETE(a) { if (a) { delete a; a=NULL; } }
#define SAFE_RELEASE(a) { if (a) { a->Release(); a=NULL; } }
//------------------------------------------------------------------------
// ����Ϣͷ(����,�鷺ָĿ¼���ļ�)
typedef struct _BLOCKINFO				// 72bytes
{
	DWORD	dwIdentify;					// ����Ϣͷ��ʶ( FKBK // BLCK = 1262701634)
	BOOL	bIsValid;					// �ÿ��Ƿ���Ч(�Ƿ���Ϊɾ��״̬)
	union{
		BOOL	bCompress;				// �ÿ��Ƿ�ѹ��(�����ļ�)
		int		nBlockDepth;			// �����(����Ŀ¼��νṹ)
	} u;
	char	cbName[MAX_BLOCK_NAME_LEN];	// ����(Ŀ¼�����ļ���)(<32)
	DWORD	dwBlockSize;				// �ÿ�ԭʼ�ߴ�(Ŀ¼ʱΪ0)
	DWORD	dwCompSize;					// �ÿ�ѹ����ߴ�(Ŀ¼ʱΪ0)
	DWORD	dwOffset;					// �ÿ����ݵ�ƫ����(��������,��ָ����λ��;Ŀ¼ʱΪ0)
	DWORD	dwPrevOffset;				// ǰһ�����ƫ����
	DWORD	dwNextOffset;				// ��һ�����ƫ����
	DWORD	dwType;						// ������(��Ŀ¼�����ļ�)
	DWORD	dwVersion;					// �汾

	_BLOCKINFO(){memset(this, 0, sizeof(_BLOCKINFO));dwIdentify='FKFS';}
	BOOL IsValid()
	{
		if (dwIdentify == 'FKFS')
			return TRUE;
		TraceLn("���󣺵�ǰ�ļ�����������һ����Ч��FreeKnightBlockͷ�ṹ��");
		return FALSE;
	}
	void SetBlockName(LPCSTR szName)
	{
		lstrcpyA(cbName, szName);
		if (lstrlenA(szName) >= MAX_BLOCK_NAME_LEN)
		{
			TraceLn("�����ļ��� %s ���ƴ�С����������32�ֽ���", szName);
			cbName[MAX_BLOCK_NAME_LEN - 1] = '\0';
		}
	}
} BLOCKINFO;
//------------------------------------------------------------------------
// ���ļ�ͷ
typedef struct _PACKHEADER	// 28bytes
{
	DWORD	dwIdentify;		// ��ʶ( FKPK //PACK = 1262698832)
	DWORD	dwVersion;		// �汾(0.0.0.1)
	DWORD	dwBlockCount;	// ����
	DWORD	dwBlockOffset;	// ��ƫ��
	DWORD	dwMapOffset;	// ӳ���ƫ��
	DWORD	dwReserved1;	// ����1
	DWORD	dwReserved2;	// ����2

	_PACKHEADER(){memset(this, 0, sizeof(_PACKHEADER));dwIdentify='KPKF';}
	BOOL IsValid()
	{
		if (dwIdentify == 'KPKF')
			return TRUE;
		TraceLn("���󣺵�ǰ���ݲ���һ����Ч��FreeKnightPackageͷ�ṹ��");
		return FALSE;
	}
} PACKHEADER;
//------------------------------------------------------------------------
// �ļ�ӳ���ṹ
typedef struct _MAPTABLE	// 64+4 btyes
{
	char	name[MAX_RELATIVE_PATH_LEN];
	DWORD	offset;

	_MAPTABLE()
	{
		memset(this, 0, sizeof(_MAPTABLE));
	}
	_MAPTABLE(LPCSTR szName, DWORD dwOffset)
	{
		memset(this, 0, sizeof(_MAPTABLE));
		assert(lstrlenA(szName) < MAX_RELATIVE_PATH_LEN);
		lstrcpyA(name, szName);
		offset = dwOffset;
	}
} MAPTABLE;
//------------------------------------------------------------------------